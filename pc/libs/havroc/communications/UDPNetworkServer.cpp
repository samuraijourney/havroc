/*
* UDPNetwork.cpp
*
*  Created on: Jan 29, 2015
*      Author: Akram
*/

#include <iostream>

#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

int UDPNetworkServer::send(std::string msg)
{
	if (m_active)
	{
		boost::shared_ptr<std::string> message(new std::string(msg));
		m_socket.async_send_to(boost::asio::buffer(*message), m_broadcast_endpoint,
			boost::bind(&UDPNetworkServer::handle_send, this, message,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	return 0;
}

void UDPNetworkServer::handle_send(boost::shared_ptr<std::string> msg/*message*/,
	const boost::system::error_code& /*error*/,
	std::size_t /*bytes_transferred*/)
{
	on_sent(*msg);
}

} /* namespace havroc */
