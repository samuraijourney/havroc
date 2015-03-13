#include <havroc/communications/UDPNetwork.h>
#include <havroc/common/CommandBuilder.h>

namespace havroc
{

	UDPNetworkClient::UDPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: UDPNetwork(service, signals_pack){}

	UDPNetworkClient::~UDPNetworkClient(){}

	int UDPNetworkClient::start_service()
	{
		m_cancel = false;

		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		m_socket = boost::shared_ptr<udp::socket>(new udp::socket(m_service, udp::endpoint(udp::v4(), UDP_PORT)));

		on_connect();

		receive();

		return SUCCESS;
	}

	void UDPNetworkClient::receive()
	{
		if (is_active())
		{
			udp::endpoint sender_endpoint;

			m_socket->async_receive_from(
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
				BYTE* start = 0;
				size_t size = 0;
				uint16_t data_size = 0;

				int i = 0;

				while (i < (int)bytes)
				{
					if (m_buffer[i] == (char)START_SYNC)
					{
						data_size = ((((uint16_t)m_buffer[i + 3]) << 8) & 0xFF00) | (((uint16_t)m_buffer[i + 4]) & 0x00FF);
						size = data_size + OVERHEAD_BYTES_CNT;
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

} /* namespace havroc */