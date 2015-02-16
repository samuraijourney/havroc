#include <havroc/communications/TCPNetwork.h>

namespace havroc
{

	TCPNetworkClient::TCPNetworkClient(boost::asio::io_service& service, std::string ip, boost::shared_ptr<comm_signals_pack> signals_pack)
		: TCPNetwork(service, signals_pack) 
	{
		set_ip(ip);
	}

	TCPNetworkClient::TCPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: TCPNetwork(service, signals_pack) 
	{
		set_ip(CC3200_IP);
	}

	TCPNetworkClient::~TCPNetworkClient(){}

	int TCPNetworkClient::set_ip(std::string ip)
	{
		if (!is_active())
		{
			m_ip = ip;
			m_endpoint = tcp::endpoint(boost::asio::ip::address::from_string(m_ip), TCP_PORT);

			return SUCCESS;
		}

		return NETWORK_IS_ACTIVE;
	}

	int TCPNetworkClient::start_service()
	{
		boost::system::error_code error;

		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		printf("\n\nTCP Client waiting for connection...\n\n");
		int handles = 1;

		m_service.reset();

		while (!is_active())
		{
			if (handles == 1)
			{
				m_socket.async_connect(m_endpoint, boost::bind(&TCPNetworkClient::handle_accept, this,
					boost::asio::placeholders::error));
			}

			handles = m_service.poll(error);
			if (error)
			{
				return NETWORK_CONNECTION_START_FAILED;
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		}

		return SUCCESS;
	}

} /* namespace havroc */