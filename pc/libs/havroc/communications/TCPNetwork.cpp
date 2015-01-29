/*
 * TCPNetwork.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

//#include "TCPNetwork.h"
#include <havroc/communications/TCPNetwork.h>

namespace havroc
{

void TCPNetwork::end_service()
{
	m_socket.close();

	on_stop();
}

int TCPNetwork::send(std::string msg)
{
	if(m_active)
	{
		boost::shared_ptr<std::string> message(new std::string(msg));
		m_socket.async_send(boost::asio::buffer(*message),
			boost::bind(&TCPNetwork::handle_send, this, message,
			boost::asio::placeholders::error,
		    boost::asio::placeholders::bytes_transferred));
	}

	return 0;
}

void TCPNetwork::handle_send(boost::shared_ptr<std::string> msg/*message*/,
							 const boost::system::error_code& /*error*/,
							 std::size_t /*bytes_transferred*/)
{
	on_sent(*msg);
}

void TCPNetwork::receive()
{
	if(m_active)
	{
		m_socket.async_receive(
			boost::asio::buffer(m_buffer, m_buffer.size()),
			boost::bind(&TCPNetwork::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
}

void TCPNetwork::handle_receive(const boost::system::error_code& error,
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

void TCPNetwork::handle_accept(const boost::system::error_code& ec)
{
	on_start();

	receive();
}

} /* namespace havroc */
