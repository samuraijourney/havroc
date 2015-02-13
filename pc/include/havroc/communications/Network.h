#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/asio.hpp>

#include <havroc/common/Errors.h>

#define CC3200_IP "192.168.43.24"

namespace havroc
{
	typedef struct _comm_signals_pack
	{
		boost::signals2::signal<void(BYTE*, size_t)> sent_event;
		boost::signals2::signal<void(BYTE*, size_t)> receive_event;
		boost::signals2::signal<void()>				 connect_event;
		boost::signals2::signal<void()>				 disconnect_event;
	} comm_signals_pack;

	class Network
	{
	public:
		Network(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~Network();

		boost::signals2::signal<void(BYTE*, size_t)>& get_sent_event()	     { return m_signals_pack->sent_event; }
		boost::signals2::signal<void(BYTE*, size_t)>& get_receive_event()	 { return m_signals_pack->receive_event; }
		boost::signals2::signal<void()>&  			  get_connect_event()	 { return m_signals_pack->connect_event; }
		boost::signals2::signal<void()>& 			  get_disconnect_event() { return m_signals_pack->disconnect_event; }

		boost::shared_ptr<comm_signals_pack> get_comm_signals_pack() { return m_signals_pack; }

		bool is_active() { return m_active; }

		int end_service(int error = SUCCESS);
		virtual int start_service() = 0;

	protected:
		void on_sent(BYTE*& msg, size_t size, bool free_mem = false) { (m_signals_pack->sent_event)(msg, size); if (free_mem) { free(msg); } }
		void on_receive(BYTE* msg, size_t size)						 { (m_signals_pack->receive_event)(msg, size); }
		void on_connect() 											 { (m_signals_pack->connect_event)(); }
		void on_disconnect() 										 { (m_signals_pack->disconnect_event)(); }

		virtual int kill_socket() = 0;

	private:
		void init_loop();
		void loop();

		boost::shared_ptr<comm_signals_pack> m_signals_pack;

		boost::asio::io_service& m_service;
		boost::thread m_poll_thread;

		bool m_active;
	};
} /* namespace havroc */

#endif /* NETWORK_H_ */
