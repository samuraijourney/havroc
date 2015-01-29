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
  m_active(false)
{
	m_start_event.connect(boost::bind(&Network::start_service, this));
	m_stop_event.connect(boost::bind(&Network::end_service, this));
}

void Network::start_service()
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
}

}
