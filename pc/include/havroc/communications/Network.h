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

typedef boost::function<void(std::string)> _sent_handler;
typedef boost::function<void(std::string)> _receive_handler;
typedef boost::function<void()> 		   _start_handler;
typedef boost::function<void()> 		   _stop_handler;

//typedef void (*_sent_handler)(std::string);
//typedef void (*_receive_handler)(std::string);
//typedef void (*_start_handler)();
//typedef void (*_stop_handler)();

template<typename T>
struct get_arg;

template<template<typename> class Func,typename Sig>
struct get_arg< Func<Sig> >
{
    typedef Sig type;
};

namespace havroc
{
	class Network
	{
	public:
		Network(boost::asio::io_service& service);
		virtual ~Network(){ m_service.stop(); m_service.reset(); }

		boost::signals2::signal<void(std::string)>& get_sent_event()    { return m_sent_event; }
		boost::signals2::signal<void(std::string)>& get_receive_event() { return m_receive_event; }
		boost::signals2::signal<void()>& 		    get_stop_event()    { return m_stop_event; }
		boost::signals2::signal<void()>&  		    get_start_event()   { return m_start_event; }

	protected:
		void on_sent	(std::string msg) { m_sent_event(msg); }
		void on_receive	(std::string msg) { m_receive_event(msg); }
		void on_start	() 				  { m_start_event(); }
		void on_stop	() 				  { m_stop_event(); }

		bool m_active;

	private:
		void start_service();
		void end_service() { m_active = false; }

		void loop();

		boost::signals2::signal<void(std::string)> m_sent_event;
		boost::signals2::signal<void(std::string)> m_receive_event;
		boost::signals2::signal<void()>   		   m_start_event;
		boost::signals2::signal<void()> 	 	   m_stop_event;

		boost::asio::io_service& m_service;
		boost::thread m_poll_thread;
	};
} /* namespace havroc */

#endif /* NETWORK_H_ */
