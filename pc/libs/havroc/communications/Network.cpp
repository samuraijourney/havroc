/*
 * UDPNetwork.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: Akram
 */

#include <havroc/communications/Network.h>

namespace havroc
{

Network::Network(boost::asio::io_service& service)
: m_service(service),
  m_active(false),
  m_reconnect(false)
{
	m_connect_event.connect(boost::bind(&Network::init_loop, this));
}

void Network::end_service()
{
	if (m_active)
	{
		m_active = false;
		m_service.stop();

		kill_socket();

		on_disconnect();
	}
}

void Network::init_loop()
{
	m_active = true;

	m_poll_thread = boost::thread(boost::bind(&Network::loop, this));
}

void Network::loop()
{
	while(m_active)
	{
		m_service.poll();
		boost::this_thread::sleep(boost::posix_time::milliseconds(50));
	}

	//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	m_service.stop();
	if (m_reconnect)
	{
		start_service();
	}
}

}
