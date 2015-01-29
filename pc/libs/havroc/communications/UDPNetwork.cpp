/*
 * UDPNetwork.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

#include <iostream>

//#include "UDPNetwork.h"
#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

UDPNetwork::UDPNetwork(boost::asio::io_service& service, int port)
: Network(service),
  m_socket(service, udp::endpoint(udp::v4(), port))
{
	m_broadcast_endpoint = udp::endpoint(boost::asio::ip::address_v4::broadcast(), UDP_PORT);
}

int UDPNetwork::start_service()
{
	on_start();

	receive();

	return 0;
}

void UDPNetwork::end_service()
{
	on_stop();
}

int UDPNetwork::broadcast(std::string msg)
{
	return send(msg);
}

int UDPNetwork::send(std::string msg)
{
	if(m_active)
	{
		boost::shared_ptr<std::string> message(new std::string(msg));
		m_socket.async_send_to(boost::asio::buffer(*message), m_broadcast_endpoint,
			boost::bind(&UDPNetwork::handle_send, this, message,
			boost::asio::placeholders::error,
		    boost::asio::placeholders::bytes_transferred));
	}

	return 0;
}

void UDPNetwork::handle_send(boost::shared_ptr<std::string> msg/*message*/,
							 const boost::system::error_code& /*error*/,
							 std::size_t /*bytes_transferred*/)
{
	on_sent(*msg);
}

void UDPNetwork::receive()
{
	if(m_active)
	{
		udp::endpoint sender_endpoint;

		m_socket.async_receive_from(
			boost::asio::buffer(m_buffer, m_buffer.size()), sender_endpoint,
			boost::bind(&UDPNetwork::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
}

void UDPNetwork::handle_receive(const boost::system::error_code& error,
							    std::size_t bytes)
{
  if (!error || error == boost::asio::error::message_size)
  {
	  std::string msg(m_buffer.begin(), m_buffer.end());
	  on_receive(msg);

	  receive();
  }
  else
  {
	  m_active = false;
  }
}

} /* namespace havroc */
