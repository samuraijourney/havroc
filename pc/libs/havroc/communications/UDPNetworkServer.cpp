#include <iostream>

#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

int UDPNetworkServer::send(char* msg, size_t size)
{
	if (is_active())
	{
		m_socket.async_send_to(boost::asio::buffer(msg,size), m_broadcast_endpoint,
			boost::bind(&UDPNetworkServer::handle_send, this, msg, size,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	return 0;
}

void UDPNetworkServer::handle_send(char* msg /*message*/,
								   size_t size /*message size*/,
								   const boost::system::error_code& /*error*/,
								   std::size_t /*bytes_transferred*/)
{
	on_sent(msg, size);
}

} /* namespace havroc */
