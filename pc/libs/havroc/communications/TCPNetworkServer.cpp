#include <havroc/communications/TCPNetwork.h>

namespace havroc
{

	TCPNetworkServer::TCPNetworkServer(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: TCPNetwork(service, signals_pack), m_acceptor(service){}

	TCPNetworkServer::~TCPNetworkServer(){}

	int TCPNetworkServer::start_service()
	{
		boost::system::error_code error;

		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		tcp::endpoint endpoint = tcp::endpoint(tcp::v4(), TCP_PORT);

		m_acceptor.open(endpoint.protocol(), error);
		if (error)
		{
			return NETWORK_CONNECTION_START_FAILED;
		}
		m_acceptor.bind(endpoint);
		if (error)
		{
			return NETWORK_CONNECTION_START_FAILED;
		}
		m_acceptor.listen();

		printf("\n\nTCP Server waiting for connection...\n\n");

		int handles = 1;

		m_service.reset();

		while (!is_active())
		{
			if (handles == 1)
			{
				m_acceptor.async_accept(m_socket,
					boost::bind(&TCPNetworkServer::handle_accept, this,
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