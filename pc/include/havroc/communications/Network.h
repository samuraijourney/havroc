#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/asio.hpp>

#include <havroc/common/Errors.h>

#define CC3200_IP_R "192.168.43.24"
#define CC3200_IP_L "192.168.43.23"

class Network;

namespace havroc
{
	typedef struct _comm_signals_pack
	{
		boost::signals2::signal<void(BYTE*, size_t)> sent_event;
		boost::signals2::signal<void(BYTE*, size_t)> receive_event;
		boost::signals2::signal<void(Network*)>		 connect_event;
		boost::signals2::signal<void(Network*)>		 disconnect_event;
	} comm_signals_pack;

	class Network
	{
	public:
		Network(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~Network();

		template<class T>
		void register_sent_callback(void(T::*sent_callback)(BYTE* msg, size_t size), T* obj)
		{
			m_signals_pack->sent_event.connect(boost::bind(sent_callback, obj, _1, _2));
		}

		void register_sent_callback(void(*sent_callback)(BYTE* msg, size_t size))
		{
			m_signals_pack->sent_event.connect(sent_callback);
		}

		template<class T>
		void register_receive_callback(void(T::*receive_callback)(BYTE* msg, size_t size), T* obj)
		{
			m_signals_pack->receive_event.connect(boost::bind(receive_callback, obj, _1, _2));
		}

		void register_receive_callback(void(*receive_callback)(BYTE* msg, size_t size))
		{
			m_signals_pack->receive_event.connect(receive_callback);
		}

		template<class T>
		void register_connect_callback(void(T::*connect_callback)(Network* network), T* obj)
		{
			m_signals_pack->connect_event.connect(boost::bind(connect_callback, obj, _1));
		}

		void register_connect_callback(void(*connect_callback)(Network* network))
		{
			m_signals_pack->connect_event.connect(connect_callback);
		}

		template<class T>
		void register_disconnect_callback(void(T::*disconnect_callback)(Network* network), T* obj)
		{
			m_signals_pack->disconnect_event.connect(boost::bind(disconnect_callback, obj, _1));
		}

		void register_disconnect_callback(void(*disconnect_callback)(Network* network))
		{
			m_signals_pack->disconnect_event.connect(disconnect_callback);
		}

		template<class T>
		void unregister_sent_callback(void(T::*sent_callback)(BYTE* msg, size_t size), T* obj)
		{
			m_signals_pack->sent_event.disconnect(boost::bind(sent_callback, obj, _1, _2));
		}

		void unregister_sent_callback(void(*sent_callback)(BYTE* msg, size_t size))
		{
			m_signals_pack->sent_event.disconnect(sent_callback);
		}

		template<class T>
		void unregister_receive_callback(void(T::*receive_callback)(BYTE* msg, size_t size), T* obj)
		{
			m_signals_pack->receive_event.disconnect(boost::bind(receive_callback, obj, _1, _2));
		}

		void unregister_receive_callback(void(*receive_callback)(BYTE* msg, size_t size))
		{
			m_signals_pack->receive_event.disconnect(receive_callback);
		}

		template<class T>
		void unregister_connect_callback(void(T::*connect_callback)(Network* network), T* obj)
		{
			m_signals_pack->connect_event.disconnect(boost::bind(connect_callback, obj, _1));
		}

		void unregister_connect_callback(void(*connect_callback)(Network* network))
		{
			m_signals_pack->connect_event.disconnect(connect_callback);
		}

		template<class T>
		void unregister_disconnect_callback(void(T::*disconnect_callback)(Network* network), T* obj)
		{
			m_signals_pack->disconnect_event.disconnect(boost::bind(disconnect_callback, obj, _1));
		}

		void unregister_disconnect_callback(void(*disconnect_callback)(Network* network))
		{
			m_signals_pack->disconnect_event.disconnect(disconnect_callback);
		}

		boost::shared_ptr<comm_signals_pack> get_comm_signals_pack() { return m_signals_pack; }

		bool is_active() { return m_active; }
		void cancel() { m_cancel = true; }

		int end_service(int error = SUCCESS);
		virtual int start_service(){};

	protected:
		void on_sent(BYTE*& msg, size_t size, bool free_mem = false) { (m_signals_pack->sent_event)(msg, size); if (free_mem) { free(msg); } }
		void on_receive(BYTE* msg, size_t size)						 { (m_signals_pack->receive_event)(msg, size); }
		void on_connect() 											 { (m_signals_pack->connect_event)(this); }
		void on_disconnect() 										 { (m_signals_pack->disconnect_event)(this); }

		virtual int kill_socket(){};

		boost::asio::io_service& m_service;

		bool m_cancel;

	private:
		void init_loop();
		void loop();

		boost::shared_ptr<comm_signals_pack> m_signals_pack;

		boost::thread m_poll_thread;

		bool m_active;
	};
} /* namespace havroc */

#endif /* NETWORK_H_ */
