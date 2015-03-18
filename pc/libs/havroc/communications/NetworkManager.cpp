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
		m_tcp_server	   = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server));
		m_tcp_client_right = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client));
		m_tcp_client_left  = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client));
		m_udp_server	   = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server));
		m_udp_client	   = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client));

		m_tcp_server->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_server, this);
		m_tcp_client_right->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client, this);
		m_tcp_client_left->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client, this);
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

	int NetworkManager::start_tcp_client(std::string ip)
	{
		int ret = NETWORK_IS_ACTIVE;

		m_stop = false;

		if (!m_tcp_client_right->is_connecting && !m_tcp_client_right->is_active())
		{
			m_tcp_client_right->set_connection(ip, TCP_CLIENT_PORT_R);
			ret = m_tcp_client_right->start_service();
		}
		else if (!m_tcp_client_left->is_connecting && !m_tcp_client_left->is_active())
		{
			m_tcp_client_left->set_connection(ip, TCP_CLIENT_PORT_L);
			ret = m_tcp_client_left->start_service();
		}
		
		return ret;
	}

	void NetworkManager::async_start_tcp_client(std::string ip)
	{
		if (ip == "")
		{
			m_async_tcp_client_right_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client, this, CC3200_IP_R));
			m_async_tcp_client_left_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client, this, CC3200_IP_L));
		}
		else
		{
			m_async_tcp_client_right_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client, this, ip));
			m_async_tcp_client_left_connection_thread = boost::thread(boost::bind(&NetworkManager::start_tcp_client, this, ip));
		}
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
		reset_tcp_server(m_tcp_server, false);

		return ret;
	}

	int NetworkManager::stop_tcp_client()
	{
		int ret1 = 0;
		int ret2 = 0;

		m_stop = true;

		if ((ret1 = m_tcp_client_right->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_tcp_client_right_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_tcp_client_right_connection_thread.interrupt();
			}

			m_tcp_client_right->cancel();
		}

		if ((ret2 = m_tcp_client_left->end_service()) == NETWORK_IS_INACTIVE)
		{
			if (!m_async_tcp_client_left_connection_thread.timed_join(boost::posix_time::milliseconds(0)))
			{
				m_async_tcp_client_left_connection_thread.interrupt();
			}

			m_tcp_client_left->cancel();
		}

		m_async_tcp_client_right_connection_thread.join();
		m_async_tcp_client_left_connection_thread.join();

		reset_tcp_client(m_tcp_client_right, false);
		reset_tcp_client(m_tcp_client_left, false);

		return ret1 > ret2 ? ret1 : ret2;
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
		reset_udp_server(m_udp_server, false);

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
		reset_udp_client(m_udp_client, false);

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
			failures += m_tcp_client_right->send(msg, size, free_mem) == SUCCESS ? 0 : 1;
			failures += m_tcp_client_left->send(msg, size, free_mem) == SUCCESS ? 0 : 1;
		}
		if (types & UDP_SERVER)
		{
			failures += m_udp_server->broadcast(msg, size, free_mem) == SUCCESS ? 0 : 1;
		}

		return types != 0 ? failures : NETWORK_CONNECTION_NOT_SET;
	}

	void NetworkManager::network_disconnect_tcp_server(Network* network)
	{
		if (m_reconnect && !m_stop)
		{
			reset_tcp_server(m_tcp_server, true);
			boost::thread(boost::bind(&TCPNetworkServer::start_service, network));
		}
	}

	void NetworkManager::network_disconnect_tcp_client(Network* network)
	{
		if (m_reconnect && !m_stop)
		{
			if (network == m_tcp_client_right.get())
			{
				reset_tcp_client(m_tcp_client_right, true);
				boost::thread(boost::bind(&TCPNetworkClient::start_service, m_tcp_client_right));
			}
			else if (network == m_tcp_client_left.get())
			{
				reset_tcp_client(m_tcp_client_left, true);
				boost::thread(boost::bind(&TCPNetworkClient::start_service, m_tcp_client_left));
			}
		}
	}

	void NetworkManager::network_disconnect_udp_server(Network* network)
	{
		if (m_reconnect && !m_stop)
		{
			reset_udp_server(m_udp_server, true);
			boost::thread(boost::bind(&UDPNetworkServer::start_service, network));
		}
	}

	void NetworkManager::network_disconnect_udp_client(Network* network)
	{	
		if (m_reconnect && !m_stop)
		{
			reset_udp_client(m_udp_client, true);
			boost::thread(boost::bind(&UDPNetworkClient::start_service, network));
		}
	}

	void NetworkManager::reset_tcp_server(boost::shared_ptr<TCPNetworkServer> network, bool keep)
	{
		_io_service_tcp_server.stop();
		_io_service_tcp_server.reset();

		auto signals_pack = network->get_comm_signals_pack();

		network.reset();

		if (keep)
		{
			network = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server, signals_pack));
		}
		else
		{
			network = boost::shared_ptr<TCPNetworkServer>(new TCPNetworkServer(_io_service_tcp_server));

			network->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_server, this);
		}

		m_reset_tcp_server_event(keep);
	}

	void NetworkManager::reset_tcp_client(boost::shared_ptr<TCPNetworkClient> network, bool keep)
	{
		_io_service_tcp_client.stop();
		_io_service_tcp_client.reset();

		auto signals_pack = network->get_comm_signals_pack();

		std::string ip = network->get_ip();
		int port	   = network->get_port();

		network.reset();

		if (keep)
		{
			network = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client, ip, port, signals_pack));
		}
		else
		{
			network = boost::shared_ptr<TCPNetworkClient>(new TCPNetworkClient(_io_service_tcp_client, ip, port));

			network->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_tcp_client, this);
		}

		m_reset_tcp_client_event(keep);
	}

	void NetworkManager::reset_udp_server(boost::shared_ptr<UDPNetworkServer> network, bool keep)
	{
		_io_service_udp_server.stop();
		_io_service_udp_server.reset();

		auto signals_pack = network->get_comm_signals_pack();

		network.reset();

		if (keep)
		{
			network = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server, signals_pack));
		}
		else
		{
			network = boost::shared_ptr<UDPNetworkServer>(new UDPNetworkServer(_io_service_udp_server));

			network->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_server, this);
		}

		m_reset_udp_server_event(keep);
	}

	void NetworkManager::reset_udp_client(boost::shared_ptr<UDPNetworkClient> network, bool keep)
	{
		_io_service_udp_client.stop();
		_io_service_udp_client.reset();

		auto signals_pack = network->get_comm_signals_pack();

		network.reset();

		if (keep)
		{
			network = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client, signals_pack));
		}
		else
		{
			network = boost::shared_ptr<UDPNetworkClient>(new UDPNetworkClient(_io_service_udp_client));

			network->register_disconnect_callback<NetworkManager>(&NetworkManager::network_disconnect_udp_client, this);
		}

		m_reset_udp_client_event(keep);
	}

}