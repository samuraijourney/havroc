#include <havroc\communications\NetworkManager.h>
#include <havroc\common\Logger.h>

namespace havroc
{

	NetworkManager* NetworkManager::m_instance = 0;

	boost::asio::io_service _io_service_tcp_server;
	boost::asio::io_service _io_service_tcp_client_left;
	boost::asio::io_service _io_service_tcp_client_right;
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
		m_tcp_server	   = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server));
		m_tcp_client_right = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client_right));
		m_tcp_client_left  = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client_left));
		m_udp_server	   = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server));
		m_udp_client	   = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client));

		m_tcp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_server, this);
		m_tcp_client_right->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client_right, this);
		m_tcp_client_left->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client_left, this);
		m_udp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_server, this);
		m_udp_client->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_client, this);
	}

	int NetworkManager::start_tcp_server(int port)
	{
		m_stop = false;

		m_tcp_server->set_port(port);

		return m_tcp_server->start_service();
	}

	void NetworkManager::async_start_tcp_server(int port)
	{
		m_async_tcp_server_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_server, this, port));
	}

	int NetworkManager::start_tcp_client(std::string ip)
	{
		return start_tcp_client_right();
	}

	int NetworkManager::start_tcp_client_left(std::string ip)
	{
		m_stop = false;

		m_tcp_client_left->set_connection(ip, TCP_CLIENT_PORT_L);

		return m_tcp_client_left->start_service();
	}

	int NetworkManager::start_tcp_client_right(std::string ip)
	{
		m_stop = false;

		m_tcp_client_right->set_connection(ip, TCP_CLIENT_PORT_R);

		return m_tcp_client_right->start_service();
	}

	void NetworkManager::async_start_tcp_client_left(std::string ip)
	{
		if (ip == "")
		{
			m_async_tcp_client_left_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client_left, this, CC3200_IP_L));
		}
		else
		{
			m_async_tcp_client_left_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client_left, this, ip));
		}
	}

	void NetworkManager::async_start_tcp_client_right(std::string ip)
	{
		if (ip == "")
		{
			m_async_tcp_client_right_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client_right, this, CC3200_IP_R));
		}
		else
		{
			m_async_tcp_client_right_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client_right, this, ip));
		}
	}

	void NetworkManager::async_start_tcp_client(std::string ip)
	{
		async_start_tcp_client_right(ip);
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

	int NetworkManager::stop_tcp_client_left()
	{
		int ret = 0;

		m_stop = true;

		if ((ret = m_tcp_client_left->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_tcp_client_left_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_tcp_client_left_connection_thread.interrupt();
			}

			m_tcp_client_left->cancel();
		}

		m_async_tcp_client_left_connection_thread.join();

		reset_tcp_client_left(false);

		return ret;
	}

	int NetworkManager::stop_tcp_client_right()
	{
		int ret = 0;

		m_stop = true;

		if ((ret = m_tcp_client_right->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_tcp_client_right_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_tcp_client_right_connection_thread.interrupt();
			}

			m_tcp_client_right->cancel();
		}

		m_async_tcp_client_right_connection_thread.join();

		reset_tcp_client_right(false);

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
		BYTE* temp = 0;

		if (types & TCP_SERVER)
		{
			temp = (BYTE*)malloc(sizeof(BYTE)*size);
			memcpy(temp, msg, size);
			
			failures += m_tcp_server->send(temp, size, true) == SUCCESS ? 0 : 1;
		}
		if (types & TCP_CLIENT_RIGHT)
		{
			temp = (BYTE*)malloc(sizeof(BYTE)*size);
			memcpy(temp, msg, size);
			
			failures += m_tcp_client_right->send(temp, size, true) == SUCCESS ? 0 : 1;
		}
		if (types & TCP_CLIENT_LEFT)
		{
			temp = (BYTE*)malloc(sizeof(BYTE)*size);
			memcpy(temp, msg, size);
			
			failures += m_tcp_client_left->send(temp, size, true) == SUCCESS ? 0 : 1;
		}
		if (types & UDP_SERVER)
		{
			temp = (BYTE*)malloc(sizeof(BYTE)*size);
			memcpy(temp, msg, size);
			
			failures += m_udp_server->broadcast(temp, size, true) == SUCCESS ? 0 : 1;
		}

		if (free_mem && msg) 
		{ 
			free(msg); 
		}

		return types != 0 ? failures : NETWORK_CONNECTION_NOT_SET;
	}

	void NetworkManager::network_disconnect_tcp_server()
	{
		if (m_reconnect && !m_stop)
		{
			LOG(LOG_INFO, "NetworkManager attempting to reconnect tcp server\n");

			reset_tcp_server(true);
			boost::thread(boost::bind(&TCPNetworkServer::start_service, m_tcp_server));
		}
	}

	void NetworkManager::network_disconnect_tcp_client_left()
	{
		if (m_reconnect && !m_stop)
		{
			LOG(LOG_INFO, "NetworkManager attempting to reconnect tcp client left\n");

			reset_tcp_client_left(true);
			boost::thread(boost::bind(&TCPNetworkClient::start_service, m_tcp_client_left));
		}
	}

	void NetworkManager::network_disconnect_tcp_client_right()
	{
		if (m_reconnect && !m_stop)
		{
			LOG(LOG_INFO, "NetworkManager attempting to reconnect tcp client right\n");

			reset_tcp_client_right(true);
			boost::thread(boost::bind(&TCPNetworkClient::start_service, m_tcp_client_right));
		}
	}

	void NetworkManager::network_disconnect_udp_server()
	{
		if (m_reconnect && !m_stop)
		{
			LOG(LOG_INFO, "NetworkManager attempting to reconnect udp server\n");

			reset_udp_server(true);
			boost::thread(boost::bind(&UDPNetworkServer::start_service, m_udp_server));
		}
	}

	void NetworkManager::network_disconnect_udp_client()
	{	
		if (m_reconnect && !m_stop)
		{
			LOG(LOG_INFO, "NetworkManager attempting to reconnect udp client\n");

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

			m_tcp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_server, this);
		}

		m_reset_tcp_server_event(keep);
	}

	void NetworkManager::reset_tcp_client_left(bool keep)
	{
		_io_service_tcp_client_left.stop();
		_io_service_tcp_client_left.reset();

		auto signals_pack = m_tcp_client_left->get_comm_signals_pack();

		std::string ip = m_tcp_client_left->get_ip();
		int port = m_tcp_client_left->get_port();

		m_tcp_client_left.reset();

		if (keep)
		{
			m_tcp_client_left = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client_left, ip, port, signals_pack));
		}
		else
		{
			m_tcp_client_left = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client_left, ip, port));

			m_tcp_client_left->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client_left, this);
		}

		m_reset_tcp_client_left_event(keep);
	}

	void NetworkManager::reset_tcp_client_right(bool keep)
	{
		_io_service_tcp_client_right.stop();
		_io_service_tcp_client_right.reset();

		auto signals_pack = m_tcp_client_right->get_comm_signals_pack();

		std::string ip = m_tcp_client_right->get_ip();
		int port = m_tcp_client_right->get_port();

		m_tcp_client_right.reset();

		if (keep)
		{
			m_tcp_client_right = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client_right, ip, port, signals_pack));
		}
		else
		{
			m_tcp_client_right = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client_right, ip, port));

			m_tcp_client_right->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client_right, this);
		}

		m_reset_tcp_client_right_event(keep);
	}

	void NetworkManager::reset_udp_server(bool keep)
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

			m_udp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_server, this);
		}

		m_reset_udp_server_event(keep);
	}

	void NetworkManager::reset_udp_client(bool keep)
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

			m_udp_client->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_client, this);
		}

		m_reset_udp_client_event(keep);
	}

}