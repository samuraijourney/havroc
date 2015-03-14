#include <havroc\communications\NetworkManager.h>

namespace havroc
{

	NetworkManager* NetworkManager::m_instance = 0;

	boost::asio::io_service _io_service_tcp_server;
	boost::asio::io_service _io_service_tcp_client;
	boost::asio::io_service _io_service_udp_server;
	boost::asio::io_service _io_service_udp_client;

	NetworkManager* NetworkManager::get()
	{
		if (m_instance == 0)
		{
			m_instance = new NetworkManager();
		}

		return m_instance;
	}

	NetworkManager::NetworkManager() 
		: m_reconnect(false),
		  m_stop(false)
	{
		m_tcp_server = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server));
		m_tcp_client = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client));
		m_udp_server = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server));
		m_udp_client = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client));

		m_tcp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_server, this);
		m_tcp_client->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client, this);
		m_udp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_server, this);
		m_udp_client->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_client, this);
	}

	int NetworkManager::start_tcp_server()
	{
		m_stop = false;

		return m_tcp_server->start_service();
	}

	void NetworkManager::async_start_tcp_server()
	{
		m_async_tcp_server_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_server, this));
	}

	int NetworkManager::start_tcp_client(char* ip)
	{
		m_stop = false;

		m_tcp_client->set_ip(ip);
		return m_tcp_client->start_service();
	}

	void NetworkManager::async_start_tcp_client(char* ip)
	{
		if (!ip)
		{
			ip = CC3200_IP;
		}

		m_async_tcp_client_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client, this, ip));
	}

	int NetworkManager::start_udp_server()
	{
		m_stop = false;

		return m_udp_server->start_service();
	}

	void NetworkManager::async_start_udp_server()
	{
		m_async_udp_server_connection_thread = boost::thread(boost::bind(&NetworkManager::start_udp_server, this));
	}

	int NetworkManager::start_udp_client()
	{
		m_stop = false;

		return m_udp_client->start_service();
	}

	void NetworkManager::async_start_udp_client()
	{
		m_async_udp_client_connection_thread = boost::thread(boost::bind(&NetworkManager::start_udp_client, this));
	}

	int NetworkManager::stop_tcp_server()
	{
		int ret;

		m_stop = true;

		if ((ret = m_tcp_server->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_tcp_server_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_tcp_server_connection_thread.interrupt();
			}

			m_tcp_server->cancel();
		}

		m_async_tcp_server_connection_thread.join();
		reset_tcp_server(false);

		return ret;
	}

	int NetworkManager::stop_tcp_client()
	{
		int ret;

		m_stop = true;

		if ((ret = m_tcp_client->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_tcp_client_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_tcp_client_connection_thread.interrupt();
			}

			m_tcp_client->cancel();
		}

		m_async_tcp_client_connection_thread.join();
		reset_tcp_client(false);

		return ret;
	}

	int NetworkManager::stop_udp_server()
	{
		int ret;

		m_stop = true;

		if ((ret = m_udp_server->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_udp_server_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_udp_server_connection_thread.interrupt();
			}

			m_udp_server->cancel();
		}

		m_async_udp_server_connection_thread.join();
		reset_udp_server(false);

		return ret;
	}

	int NetworkManager::stop_udp_client()
	{
		int ret;

		m_stop = true;

		if ((ret = m_udp_client->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_udp_client_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_udp_client_connection_thread.interrupt();
			}

			m_udp_client->cancel();
		}

		m_async_udp_client_connection_thread.join();
		reset_udp_client(false);

		return ret;
	}

	int NetworkManager::send(std::string msg)
	{
		return send(msg, m_desired_connections);
	}

	int NetworkManager::send(std::string msg, uint8_t types)
	{
		BYTE* c_msg = (BYTE*)malloc(sizeof(BYTE)*(msg.length() + 1));
		strcpy((char*)c_msg, msg.c_str());

		return send(c_msg, msg.length() + 1, types, true);
	}

	int NetworkManager::send(BYTE* msg, size_t size, bool free_mem)
	{
		return send(msg, size, m_desired_connections, free_mem);
	}

	int NetworkManager::send(BYTE* msg, size_t size, uint8_t types, bool free_mem)
	{
		int failures = 0;

		if (types & TCP_SERVER)
		{
			failures += m_tcp_server->send(msg, size, free_mem) == SUCCESS ? 0 : 1;
		}
		if (types & TCP_CLIENT)
		{
			failures += m_tcp_client->send(msg, size, free_mem) == SUCCESS ? 0 : 1;
		}
		if (types & UDP_SERVER)
		{
			failures += m_udp_server->broadcast(msg, size, free_mem) == SUCCESS ? 0 : 1;
		}

		return types != 0 ? failures : NETWORK_CONNECTION_NOT_SET;
	}

	void NetworkManager::network_disconnect_tcp_server()
	{
		if (m_reconnect && !m_stop)
		{
			reset_tcp_server(true);
			boost::thread(boost::bind(&TCPNetworkServer::start_service, m_tcp_server));
		}
	}

	void NetworkManager::network_disconnect_tcp_client()
	{
		if (m_reconnect && !m_stop)
		{
			reset_tcp_client(true);
			boost::thread(boost::bind(&TCPNetworkClient::start_service, m_tcp_client));
		}
	}

	void NetworkManager::network_disconnect_udp_server()
	{
		if (m_reconnect && !m_stop)
		{
			reset_udp_server(true);
			boost::thread(boost::bind(&UDPNetworkServer::start_service, m_udp_server));
		}
	}

	void NetworkManager::network_disconnect_udp_client()
	{	
		if (m_reconnect && !m_stop)
		{
			reset_udp_client(true);
			boost::thread(boost::bind(&UDPNetworkClient::start_service, m_udp_client));
		}
	}

	void NetworkManager::reset_tcp_server(bool keep)
	{
		_io_service_tcp_server.stop();
		_io_service_tcp_server.reset();

		auto signals_pack = m_tcp_server->get_comm_signals_pack();

		m_tcp_server.reset();

		if (keep)
		{
			m_tcp_server = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server, signals_pack));
		}
		else
		{
			m_tcp_server = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server));
		}
	}

	void NetworkManager::reset_tcp_client(bool keep)
	{
		_io_service_tcp_client.stop();
		_io_service_tcp_client.reset();

		auto signals_pack = m_tcp_client->get_comm_signals_pack();

		std::string ip = m_tcp_client->get_ip();

		m_tcp_client.reset();

		if (keep)
		{
			m_tcp_client = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client, ip, signals_pack));
		}
		else
		{
			m_tcp_client = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client, ip));
		}
	}

	void NetworkManager::reset_udp_client(bool keep)
	{
		_io_service_udp_server.stop();
		_io_service_udp_server.reset();

		auto signals_pack = m_udp_server->get_comm_signals_pack();

		m_udp_server.reset();

		if (keep)
		{
			m_udp_server = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server, signals_pack));
		}
		else
		{
			m_udp_server = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server));
		}
	}

	void NetworkManager::reset_udp_server(bool keep)
	{
		_io_service_udp_client.stop();
		_io_service_udp_client.reset();

		auto signals_pack = m_udp_client->get_comm_signals_pack();

		m_udp_client.reset();

		if (keep)
		{
			m_udp_client = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client, signals_pack));
		}
		else
		{
			m_udp_client = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client));
		}
	}

}