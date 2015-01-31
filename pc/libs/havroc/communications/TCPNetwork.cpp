/*
 * TCPNetwork.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

#include <havroc/communications/TCPNetwork.h>
#include <havroc/communications/CommandBuilder.h>

namespace havroc
{

int TCPNetwork::send(char* msg, size_t size)
{
	if(is_active())
	{
		m_socket.async_send(boost::asio::buffer(msg,size),
			boost::bind(&TCPNetwork::handle_send, this, msg, size,
			boost::asio::placeholders::error,
		    boost::asio::placeholders::bytes_transferred));
	}

	return 0;
}

void TCPNetwork::handle_send(char* msg /*message*/,
							 size_t size /*message size*/,
							 const boost::system::error_code& /*error*/,
							 std::size_t bytes /*bytes_transferred*/)
{
	on_sent(msg, size);
}

void TCPNetwork::receive()
{
	if (is_active())
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
	if (is_active())
	{
		if (!error || error == boost::asio::error::message_size)
		{
			char* start = 0;
			size_t size = 0;

			for (int i = 0; i < (int)bytes; i++)
			{
				if (m_buffer[i] == (char)START_SYNC)
				{
					if (size > 0)
					{
						on_receive(start, size);
						size = 0;
					}

					start = &m_buffer.c_array()[i];
				}

				if (start)
				{
					size++;
				}
			}
			
			if (size > 0)
			{
				on_receive(start, size);
				size = 0;
			}

			receive();
		}
		else
		{
			end_service();
		}
	}
}

void TCPNetwork::handle_accept(const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message() << ". Trying again." << std::endl;
	}
	else
	{
		on_connect();

		receive();
	}
}

} /* namespace havroc */
