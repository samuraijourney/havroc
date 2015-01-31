#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

int UDPNetworkClient::start_service()
{
	on_connect();

	receive();

	return 0;
}

void UDPNetworkClient::receive()
{
	if (is_active())
	{
		udp::endpoint sender_endpoint;

		m_socket.async_receive_from(
			boost::asio::buffer(m_buffer, m_buffer.size()), sender_endpoint,
			boost::bind(&UDPNetworkClient::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
}

void UDPNetworkClient::handle_receive(const boost::system::error_code& error,
									  std::size_t bytes)
{
	if (is_active())
	{
		if (!error || error == boost::asio::error::message_size)
		{
			on_receive(m_buffer.c_array(), bytes);

			receive();
		}
		else
		{
			end_service();
		}
	}
}

} /* namespace havroc */