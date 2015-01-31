/*
 * UDPNetwork.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

#include <iostream>

#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

UDPNetwork::UDPNetwork(boost::asio::io_service& service, int port)
: Network(service),
  m_socket(service, udp::endpoint(udp::v4(), port)){}

int UDPNetwork::start_service()
{
	on_connect();

	return 0;
}

} /* namespace havroc */
