#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

	UDPNetwork::UDPNetwork(boost::asio::io_service& service, int port, boost::shared_ptr<comm_signals_pack> signals_pack)
		: Network(service, signals_pack),
		  m_socket(service, udp::endpoint(udp::v4(), port)){}

	UDPNetwork::~UDPNetwork(){}

	int UDPNetwork::start_service()
	{
		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		on_connect();

		return SUCCESS;
	}

	int UDPNetwork::kill_socket()
	{
		boost::system::error_code error;

		m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
		if (error)
		{
			return NETWORK_CONNECTION_END_FAILED;
		}

		m_socket.close();
		if (error)
		{
			return NETWORK_CONNECTION_END_FAILED;
		}

		return SUCCESS;
	}

} /* namespace havroc */
