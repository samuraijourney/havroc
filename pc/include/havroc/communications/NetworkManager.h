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

		template<class T>
		void register_sent_callback(void(T::*sent_callback)(char* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_sent_event().connect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_sent_event().connect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_sent_event().connect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_sent_event().connect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
		}

		template<class T>
		void register_receive_callback(void(T::*receive_callback)(char* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_receive_event().connect(boost::bind(&T::receive_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_receive_event().connect(boost::bind(&T::receive_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_receive_event().connect(boost::bind(&T::receive_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_receive_event().connect(boost::bind(&T::receive_callback, obj, _1, _2));
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
				m_tcp_server->get_connect_event().connect(boost::bind(&T::connect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_connect_event().connect(boost::bind(&T::connect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_connect_event().connect(boost::bind(&T::connect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_connect_event().connect(boost::bind(&T::connect_callback, obj));
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
				m_tcp_server->get_disconnect_event().connect(boost::bind(&T::disconnect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_disconnect_event().connect(boost::bind(&T::disconnect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_disconnect_event().connect(boost::bind(&T::disconnect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_disconnect_event().connect(boost::bind(&T::disconnect_callback, obj));
			}
		}

		template<class T>
		void unregister_sent_callback(void(T::*sent_callback)(char* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_sent_event().disconnect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_sent_event().disconnect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_sent_event().disconnect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_sent_event().disconnect(boost::bind(&T::sent_callback, obj, _1, _2));
			}
		}

		template<class T>
		void unregister_receive_callback(void(T::*receive_callback)(char* msg, size_t size), T* obj, uint8_t types = 0)
		{
			if (types == 0)
			{
				types = m_desired_connections;
			}

			if (types & TCP_SERVER)
			{
				m_tcp_server->get_receive_event().disconnect(boost::bind(&T::receive_callback, obj, _1, _2));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_receive_event().disconnect(boost::bind(&T::receive_callback, obj, _1, _2));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_receive_event().disconnect(boost::bind(&T::receive_callback, obj, _1, _2));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_receive_event().disconnect(boost::bind(&T::receive_callback, obj, _1, _2));
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
				m_tcp_server->get_connect_event().disconnect(boost::bind(&T::connect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_connect_event().disconnect(boost::bind(&T::connect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_connect_event().disconnect(boost::bind(&T::connect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_connect_event().disconnect(boost::bind(&T::connect_callback, obj));
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
				m_tcp_server->get_disconnect_event().disconnect(boost::bind(&T::disconnect_callback, obj));
			}
			if (types & TCP_CLIENT)
			{
				m_tcp_client->get_disconnect_event().disconnect(boost::bind(&T::disconnect_callback, obj));
			}
			if (types & UDP_SERVER)
			{
				m_udp_server->get_disconnect_event().disconnect(boost::bind(&T::disconnect_callback, obj));
			}
			if (types & UDP_CLIENT)
			{
				m_udp_client->get_disconnect_event().disconnect(boost::bind(&T::disconnect_callback, obj));
			}
		}

		int start_tcp_server();
		int start_tcp_client(char* ip = CC3200_IP);
		int start_udp_server();
		int start_udp_client();

		int send(std::string msg);
		int send(std::string msg, uint8_t types);
		int send(char* msg, size_t size, bool free_mem = false);
		int send(char* msg, size_t size, uint8_t types, bool free_mem = false);

		void set_connections(uint8_t connections) { m_desired_connections = connections; }

	private:
		NetworkManager();

		void network_disconnect_tcp_server();
		void network_disconnect_tcp_client();
		void network_disconnect_udp_server();
		void network_disconnect_udp_client();

		uint8_t m_desired_connections = 0;

		boost::shared_ptr<TCPNetworkServer> m_tcp_server;
		boost::shared_ptr<TCPNetworkClient> m_tcp_client;
		boost::shared_ptr<UDPNetworkServer> m_udp_server;
		boost::shared_ptr<UDPNetworkClient> m_udp_client;

		static NetworkManager* m_instance;
	};

} /* namespace havroc */

#endif /* NETWORKMANAGER_H_ */

