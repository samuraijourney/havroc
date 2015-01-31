/*
 * Network.h
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/asio.hpp>

#define CC3200_IP "192.168.43.24"

namespace havroc
{
	class Network
	{
	public:
		Network(boost::asio::io_service& service);
		virtual ~Network(){ m_service.stop(); m_service.reset(); }

		boost::signals2::signal<void(char*, size_t)>& get_sent_event()	     { return m_sent_event; }
		boost::signals2::signal<void(char*, size_t)>& get_receive_event()	 { return m_receive_event; }
		boost::signals2::signal<void()>&  			  get_connect_event()	 { return m_connect_event; }
		boost::signals2::signal<void()>& 			  get_disconnect_event() { return m_disconnect_event; }

		void set_reconnect(bool flag) { m_reconnect = flag; }

		bool is_active() { return m_active; }

		void end_service();
		virtual int start_service() = 0;

	protected:
		void on_sent(char*& msg, size_t size)   { m_sent_event(msg, size); free(msg); }
		void on_receive(char* msg, size_t size) { m_receive_event(msg, size); }
		void on_connect() 						{ m_connect_event(); }
		void on_disconnect() 					{ m_disconnect_event(); }

		virtual void kill_socket() = 0;

	private:
		void init_loop();
		void loop();

		boost::signals2::signal<void(char*, size_t)> m_sent_event;
		boost::signals2::signal<void(char*, size_t)> m_receive_event;
		boost::signals2::signal<void()>				 m_connect_event;
		boost::signals2::signal<void()> 			 m_disconnect_event;

		boost::asio::io_service& m_service;
		boost::thread m_poll_thread;

		bool m_active;
		bool m_reconnect;
	};
} /* namespace havroc */

#endif /* NETWORK_H_ */
