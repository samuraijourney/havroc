#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/Logger.h>

namespace havroc
{

	TCPNetworkServer::TCPNetworkServer(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: TCPNetwork(service, signals_pack), m_acceptor(service){}

	TCPNetworkServer::~TCPNetworkServer(){}

	int TCPNetworkServer::set_port(int port)
	{
		if (!is_active())
		{
			m_port = port;

			return SUCCESS;
		}

		return NETWORK_IS_ACTIVE;
	}

	int TCPNetworkServer::start_service()
	{
		boost::system::error_code error;

		m_cancel = false;

		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		tcp::endpoint endpoint = tcp::endpoint(tcp::v4(), m_port);

		m_acceptor.open(endpoint.protocol(), error);
		if (error)
		{
			return NETWORK_CONNECTION_START_FAILED;
		}
		m_acceptor.bind(endpoint, error);
		if (error)
		{
			return NETWORK_CONNECTION_START_FAILED;
		}
		m_acceptor.listen();

		printf("\n\n");
		Logger::log("TCP Server waiting for connection...");
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
				m_acceptor.async_accept(m_socket,
					boost::bind(&TCPNetworkServer::handle_accept, this,
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