#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include <havroc\communications\UDPNetwork.h>
#include <havroc\communications\TCPNetwork.h>

#define TCP_CLIENT 0x01 << 0
#define TCP_SERVER 0x01 << 1
#define UDP_CLIENT 0x01 << 2
#define UDP_SERVER 0x01 << 3

#define TCP_CLIENT_PORT_R	13
#define TCP_CLIENT_PORT_L	14

namespace havroc
{

	class NetworkManager
	{
	public:
		~NetworkManager(){}

		static NetworkManager* get();

		int start_tcp_server();
		int start_tcp_client(std::string ip = CC3200_IP_R);
		int start_udp_server();
		int start_udp_client();

		void async_start_tcp_server();
		void async_start_tcp_client(std::string ip = CC3200_IP_R);
		void async_start_udp_server();
		void async_start_udp_client();

		int stop_tcp_server();
		int stop_tcp_client();
		int stop_udp_server();
		int stop_udp_client();

		bool is_tcp_server_active() { return m_tcp_server->is_active(); }
		bool is_tcp_client_active() { return m_tcp_client_right->is_active() || m_tcp_client_left->is_active(); }
		bool is_udp_server_active() { return m_udp_server->is_active(); }
		bool is_udp_client_active() { return m_udp_client->is_active(); }
		bool is_active()			{ return is_tcp_server_active() || 
											 is_tcp_client_active() || 
											 is_udp_server_active() || 
											 is_udp_client_active(); }

		int send(std::string msg);
		int send(std::string msg, uint8_t types);
		int send(BYTE* msg, size_t size, bool free_mem = false);
		int send(BYTE* msg, size_t size, uint8_t types, bool free_mem = false);

		void	set_connections(uint8_t connections) { m_desired_connections = connections; }
		void	set_reconnect(bool reconnect)		 { m_reconnect = reconnect; }

		uint8_t get_connections()					 { return m_desired_connections; }
		bool	get_reconnect()						 { return m_reconnect; }

		template<class T>
		void register_sent_callback(void(T::*sent_callback)(BYTE* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_sent_callback<T>(sent_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_sent_callback<T>(sent_callback, obj);
				m_tcp_client_left->register_sent_callback<T>(sent_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_sent_callback<T>(sent_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_sent_callback<T>(sent_callback, obj);
			}
		}

		void register_sent_callback(void(*sent_callback)(BYTE* msg, size_t size), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_sent_callback(sent_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_sent_callback(sent_callback);
				m_tcp_client_left->register_sent_callback(sent_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_sent_callback(sent_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_sent_callback(sent_callback);
			}
		}

		template<class T>
		void register_receive_callback(void(T::*receive_callback)(BYTE* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_receive_callback<T>(receive_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_receive_callback<T>(receive_callback, obj);
				m_tcp_client_left->register_receive_callback<T>(receive_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_receive_callback<T>(receive_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_receive_callback<T>(receive_callback, obj);
			}
		}

		void register_receive_callback(void(*receive_callback)(BYTE* msg, size_t size), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_receive_callback(receive_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_receive_callback(receive_callback);
				m_tcp_client_left->register_receive_callback(receive_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_receive_callback(receive_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_receive_callback(receive_callback);
			}
		}

		template<class T>
		void register_connect_callback(void(T::*connect_callback)(Network* network), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_connect_callback<T>(connect_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_connect_callback<T>(connect_callback, obj);
				m_tcp_client_left->register_connect_callback<T>(connect_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_connect_callback<T>(connect_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_connect_callback<T>(connect_callback, obj);
			}
		}

		void register_connect_callback(void(*connect_callback)(Network* network), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_connect_callback(connect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_connect_callback(connect_callback);
				m_tcp_client_left->register_connect_callback(connect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_connect_callback(connect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_connect_callback(connect_callback);
			}
		}

		template<class T>
		void register_disconnect_callback(void(T::*disconnect_callback)(Network* network), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_disconnect_callback<T>(disconnect_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_disconnect_callback<T>(disconnect_callback, obj);
				m_tcp_client_left->register_disconnect_callback<T>(disconnect_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_disconnect_callback<T>(disconnect_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_disconnect_callback<T>(disconnect_callback, obj);
			}
		}

		void register_disconnect_callback(void(*disconnect_callback)(Network* network), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->register_disconnect_callback(disconnect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->register_disconnect_callback(disconnect_callback);
				m_tcp_client_left->register_disconnect_callback(disconnect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->register_disconnect_callback(disconnect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->register_disconnect_callback(disconnect_callback);
			}
		}

		template<class T>
		void unregister_sent_callback(void(T::*sent_callback)(BYTE* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_sent_callback<T>(sent_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_sent_callback<T>(sent_callback, obj);
				m_tcp_client_left->unregister_sent_callback<T>(sent_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_sent_callback<T>(sent_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_sent_callback<T>(sent_callback, obj);
			}
		}

		void unregister_sent_callback(void(*sent_callback)(BYTE* msg, size_t size), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_sent_callback(sent_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_sent_callback(sent_callback);
				m_tcp_client_left->unregister_sent_callback(sent_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_sent_callback(sent_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_sent_callback(sent_callback);
			}
		}

		template<class T>
		void unregister_receive_callback(void(T::*receive_callback)(BYTE* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_receive_callback<T>(receive_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_receive_callback<T>(receive_callback, obj);
				m_tcp_client_left->unregister_receive_callback<T>(receive_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_receive_callback<T>(receive_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_receive_callback<T>(receive_callback, obj);
			}
		}

		void unregister_receive_callback(void(*receive_callback)(BYTE* msg, size_t size), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_receive_callback(receive_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_receive_callback(receive_callback);
				m_tcp_client_left->unregister_receive_callback(receive_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_receive_callback(receive_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_receive_callback(receive_callback);
			}
		}

		template<class T>
		void unregister_connect_callback(void(T::*connect_callback)(Network* network), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_connect_callback<T>(connect_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_connect_callback<T>(connect_callback, obj);
				m_tcp_client_left->unregister_connect_callback<T>(connect_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_connect_callback<T>(connect_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_connect_callback<T>(connect_callback, obj);
			}
		}

		void unregister_connect_callback(void(*connect_callback)(Network* network), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_connect_callback(connect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_connect_callback(connect_callback);
				m_tcp_client_left->unregister_connect_callback(connect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_connect_callback(connect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_connect_callback(connect_callback);
			}
		}

		template<class T>
		void unregister_disconnect_callback(void(T::*disconnect_callback)(Network* network), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_disconnect_callback<T>(disconnect_callback, obj);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_disconnect_callback<T>(disconnect_callback, obj);
				m_tcp_client_left->unregister_disconnect_callback<T>(disconnect_callback, obj);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_disconnect_callback<T>(disconnect_callback, obj);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_disconnect_callback<T>(disconnect_callback, obj);
			}
		}

		void unregister_disconnect_callback(void(*disconnect_callback)(Network* network), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->unregister_disconnect_callback(disconnect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client_right->unregister_disconnect_callback(disconnect_callback);
				m_tcp_client_left->unregister_disconnect_callback(disconnect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->unregister_disconnect_callback(disconnect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->unregister_disconnect_callback(disconnect_callback);
			}
		}


		template<class T>
		void register_reset_callback(void(T::*reset_callback)(bool keep), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_reset_tcp_server_event.connect(boost::bind(reset_callback, obj, _1));
			}
			if (types & TCP_CLIENT)
			{
				m_reset_tcp_client_event.connect(boost::bind(reset_callback, obj, _1));
			}
			if (types & UDP_SERVER)
			{
				m_reset_udp_server_event.connect(boost::bind(reset_callback, obj, _1));
			}
			if (types & UDP_CLIENT)
			{
				m_reset_udp_client_event.connect(boost::bind(reset_callback, obj, _1));
			}
		}

		void register_reset_callback(void(*reset_callback)(bool keep), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_reset_tcp_server_event.connect(reset_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_reset_tcp_client_event.connect(reset_callback);
			}
			if (types & UDP_SERVER)
			{
				m_reset_udp_server_event.connect(reset_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_reset_udp_client_event.connect(reset_callback);
			}
		}

		template<class T>
		void unregister_reset_callback(void(T::*reset_callback)(bool keep), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_reset_tcp_server_event.disconnect(boost::bind(reset_callback, obj, _1));
			}
			if (types & TCP_CLIENT)
			{
				m_reset_tcp_client_event.disconnect(boost::bind(reset_callback, obj, _1));
			}
			if (types & UDP_SERVER)
			{
				m_reset_udp_server_event.disconnect(boost::bind(reset_callback, obj, _1));
			}
			if (types & UDP_CLIENT)
			{
				m_reset_udp_client_event.disconnect(boost::bind(reset_callback, obj, _1));
			}
		}

		void unregister_reset_callback(void(*reset_callback)(bool keep), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_reset_tcp_server_event.disconnect(reset_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_reset_tcp_client_event.disconnect(reset_callback);
			}
			if (types & UDP_SERVER)
			{
				m_reset_udp_server_event.disconnect(reset_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_reset_udp_client_event.disconnect(reset_callback);
			}
		}

	private:
		NetworkManager();

		void reset_tcp_server(boost::shared_ptr<TCPNetworkServer> network, bool keep);
		void reset_tcp_client(boost::shared_ptr<TCPNetworkClient> network, bool keep);
		void reset_udp_server(boost::shared_ptr<UDPNetworkServer> network, bool keep);
		void reset_udp_client(boost::shared_ptr<UDPNetworkClient> network, bool keep);

		void network_disconnect_tcp_server(Network* network);
		void network_disconnect_tcp_client(Network* network);
		void network_disconnect_udp_server(Network* network);
		void network_disconnect_udp_client(Network* network);

		uint8_t m_desired_connections = 0;
		bool    m_reconnect;
		bool	m_stop;

		boost::shared_ptr<TCPNetworkServer> m_tcp_server;
		boost::shared_ptr<TCPNetworkClient> m_tcp_client_left;
		boost::shared_ptr<TCPNetworkClient> m_tcp_client_right;
		boost::shared_ptr<UDPNetworkServer> m_udp_server;
		boost::shared_ptr<UDPNetworkClient> m_udp_client;

		boost::thread m_async_tcp_server_connection_thread;
		boost::thread m_async_tcp_client_left_connection_thread;
		boost::thread m_async_tcp_client_right_connection_thread;
		boost::thread m_async_udp_server_connection_thread;
		boost::thread m_async_udp_client_connection_thread;

		boost::signals2::signal<void(bool)> m_reset_tcp_server_event;
		boost::signals2::signal<void(bool)> m_reset_tcp_client_event;
		boost::signals2::signal<void(bool)> m_reset_udp_server_event;
		boost::signals2::signal<void(bool)> m_reset_udp_client_event;

		static NetworkManager* m_instance;
	};

} /* namespace havroc */

#endif /* NETWORKMANAGER_H_ */

