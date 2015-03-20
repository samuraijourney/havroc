#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/Logger.h>

namespace havroc
{

	TCPNetworkClient::TCPNetworkClient(boost::asio::io_service& service, std::string ip, int port, boost::shared_ptr<comm_signals_pack> signals_pack)
		: TCPNetwork(service, signals_pack)
	{
		set_connection(ip, port);
	}

	TCPNetworkClient::TCPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: TCPNetwork(service, signals_pack)
	{
		set_connection("127.0.0.1", DEFAULT_TCP_PORT);
	}

	TCPNetworkClient::~TCPNetworkClient(){}

	int TCPNetworkClient::set_connection(std::string ip, int port)
	{
		if (!is_active())
		{
			m_ip = ip;
			m_port = port;
			m_endpoint = tcp::endpoint(boost::asio::ip::address::from_string(m_ip), m_port);

			return SUCCESS;
		}

		return NETWORK_IS_ACTIVE;
	}

	int TCPNetworkClient::start_service()
	{
		boost::system::error_code error;

		m_cancel = false;

		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		printf("\n\n");
		LOG("TCP Client waiting for connection...");
		printf("\n\n");

		int handles = 1;

		m_service.reset();

		m_connecting = true;

		while (!is_active())
		{
			if (m_cancel)
			{
				m_connecting = false;

				return NETWORK_CONNECTION_START_CANCELLED;
			}

			if (handles == 1)
			{
				m_socket.async_connect(m_endpoint, boost::bind(&TCPNetworkClient::handle_accept, this,
					boost::asio::placeholders::error));
			}

			handles = m_service.poll(error);
			if (error)
			{
				m_connecting = false;

				return NETWORK_CONNECTION_START_FAILED;
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		}

		m_connecting = false;

		return SUCCESS;
	}

} /* namespace havroc */