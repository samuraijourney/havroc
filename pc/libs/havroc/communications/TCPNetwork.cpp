#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/CommandBuilder.h>

namespace havroc
{

	TCPNetwork::TCPNetwork(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: Network(service, signals_pack), m_socket(service){}

	TCPNetwork::~TCPNetwork(){}

	int TCPNetwork::send(BYTE* msg, size_t size, bool free_mem)
	{
		if(is_active())
		{
			m_socket.async_send(boost::asio::buffer(msg,size),
				boost::bind(&TCPNetwork::handle_send, this, msg, size, free_mem,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

			return SUCCESS;
		}

		return NETWORK_IS_INACTIVE;
	}

	void TCPNetwork::handle_send(BYTE* msg /*message*/,
								 size_t size /*message size*/,
								 bool free_mem /*ownership*/,
								 const boost::system::error_code& /*error*/,
								 std::size_t bytes /*bytes_transferred*/)
	{
		on_sent(msg, size, free_mem);
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
				BYTE* start = 0;
				size_t size = 0;
				uint16_t data_size = 0;

				int i = 0;

				while (i < (int)bytes)
				{
					if (m_buffer[i] == (char)START_SYNC)
					{
						data_size = ((((uint16_t)m_buffer[i+2]) << 8) & 0xFF00) | (((uint16_t)m_buffer[i+3]) & 0x00FF);
						size = data_size + 4;
						start = (BYTE*)&m_buffer.c_array()[i];

						on_receive(start, size);

						i += size;
					}
					else
					{
						on_receive((BYTE*)m_buffer.c_array(), bytes);
						break;
					}
				}

				receive();
			}
			else
			{
				end_service(NETWORK_DATA_RECEIVE_FAILURE);
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

	int TCPNetwork::kill_socket()
	{
		boost::system::error_code error;

		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
		if (error)
		{
			return NETWORK_CONNECTION_END_FAILED;
		}

		m_socket.close(error);
		if (error)
		{
			return NETWORK_CONNECTION_END_FAILED;
		}

		return SUCCESS;
	}

} /* namespace havroc */
