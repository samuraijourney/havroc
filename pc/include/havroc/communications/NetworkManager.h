#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include <havroc\communications\UDPNetwork.h>
#include <havroc\communications\TCPNetwork.h>

#define TCP_CLIENT 0x01 << 0
#define TCP_SERVER 0x01 << 1
#define UDP_CLIENT 0x01 << 2
#define UDP_SERVER 0x01 << 3

namespace havroc
{

	class NetworkManager
	{
	public:
		~NetworkManager(){}

		static NetworkManager* get();

		int start_tcp_server();
		int start_tcp_client(char* ip = CC3200_IP);
		int start_udp_server();
		int start_udp_client();

		int stop_tcp_server();
		int stop_tcp_client();
		int stop_udp_server();
		int stop_udp_client();

		bool is_tcp_server_active() { return m_tcp_server->is_active(); }
		bool is_tcp_client_active() { return m_tcp_client->is_active(); }
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
				m_tcp_server->get_sent_event().connect(boost::bind(sent_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_sent_event().connect(boost::bind(sent_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_sent_event().connect(boost::bind(sent_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_sent_event().connect(boost::bind(sent_callback, obj, _1, _2));
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
				m_tcp_server->get_sent_event().connect(sent_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_sent_event().connect(sent_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_sent_event().connect(sent_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_sent_event().connect(sent_callback);
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
				m_tcp_server->get_receive_event().connect(boost::bind(receive_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_receive_event().connect(boost::bind(receive_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_receive_event().connect(boost::bind(receive_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_receive_event().connect(boost::bind(receive_callback, obj, _1, _2));
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
				m_tcp_server->get_receive_event().connect(receive_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_receive_event().connect(receive_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_receive_event().connect(receive_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_receive_event().connect(receive_callback);
			}
		}

		template<class T>
		void register_connect_callback(void(T::*connect_callback)(), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_connect_event().connect(boost::bind(connect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_connect_event().connect(boost::bind(connect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_connect_event().connect(boost::bind(connect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_connect_event().connect(boost::bind(connect_callback, obj));
			}
		}

		void register_connect_callback(void(*connect_callback)(), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_connect_event().connect(connect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_connect_event().connect(connect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_connect_event().connect(connect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_connect_event().connect(connect_callback);
			}
		}

		template<class T>
		void register_disconnect_callback(void(T::*disconnect_callback)(), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_disconnect_event().connect(boost::bind(disconnect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_disconnect_event().connect(boost::bind(disconnect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_disconnect_event().connect(boost::bind(disconnect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_disconnect_event().connect(boost::bind(disconnect_callback, obj));
			}
		}

		void register_disconnect_callback(void(*disconnect_callback)(), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_disconnect_event().connect(disconnect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_disconnect_event().connect(disconnect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_disconnect_event().connect(disconnect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_disconnect_event().connect(disconnect_callback);
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
				m_tcp_server->get_sent_event().disconnect(boost::bind(sent_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_sent_event().disconnect(boost::bind(sent_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_sent_event().disconnect(boost::bind(sent_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_sent_event().disconnect(boost::bind(sent_callback, obj, _1, _2));
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
				m_tcp_server->get_sent_event().disconnect(sent_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_sent_event().disconnect(sent_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_sent_event().disconnect(sent_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_sent_event().disconnect(sent_callback);
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
				m_tcp_server->get_receive_event().disconnect(boost::bind(receive_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_receive_event().disconnect(boost::bind(receive_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_receive_event().disconnect(boost::bind(receive_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_receive_event().disconnect(boost::bind(receive_callback, obj, _1, _2));
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
				m_tcp_server->get_receive_event().disconnect(receive_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_receive_event().disconnect(receive_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_receive_event().disconnect(receive_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_receive_event().disconnect(receive_callback);
			}
		}

		template<class T>
		void unregister_connect_callback(void(T::*connect_callback)(), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_connect_event().disconnect(boost::bind(connect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_connect_event().disconnect(boost::bind(connect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_connect_event().disconnect(boost::bind(connect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_connect_event().disconnect(boost::bind(connect_callback, obj));
			}
		}

		void unregister_connect_callback(void(*connect_callback)(), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_connect_event().disconnect(connect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_connect_event().disconnect(connect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_connect_event().disconnect(connect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_connect_event().disconnect(connect_callback);
			}
		}

		template<class T>
		void unregister_disconnect_callback(void(T::*disconnect_callback)(), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_disconnect_event().disconnect(boost::bind(disconnect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_disconnect_event().disconnect(boost::bind(disconnect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_disconnect_event().disconnect(boost::bind(disconnect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_disconnect_event().disconnect(boost::bind(disconnect_callback, obj));
			}
		}

		void unregister_disconnect_callback(void(*disconnect_callback)(), uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_disconnect_event().disconnect(disconnect_callback);
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_disconnect_event().disconnect(disconnect_callback);
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_disconnect_event().disconnect(disconnect_callback);
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_disconnect_event().disconnect(disconnect_callback);
			}
		}

	private:
		NetworkManager();

		void network_disconnect_tcp_server();
		void network_disconnect_tcp_client();
		void network_disconnect_udp_server();
		void network_disconnect_udp_client();

		uint8_t m_desired_connections = 0;
		bool    m_reconnect;
		bool	m_stop;

		boost::shared_ptr<TCPNetworkServer> m_tcp_server;
		boost::shared_ptr<TCPNetworkClient> m_tcp_client;
		boost::shared_ptr<UDPNetworkServer> m_udp_server;
		boost::shared_ptr<UDPNetworkClient> m_udp_client;

		static NetworkManager* m_instance;
	};

} /* namespace havroc */

#endif /* NETWORKMANAGER_H_ */
