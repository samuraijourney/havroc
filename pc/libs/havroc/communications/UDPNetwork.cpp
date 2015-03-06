#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

	UDPNetwork::UDPNetwork(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: Network(service, signals_pack){}

	UDPNetwork::~UDPNetwork(){}

	int UDPNetwork::kill_socket()
	{
		boost::system::error_code error;

		m_socket->shutdown(boost::asio::ip::udp::socket::shutdown_both);
		if (error)
		{
			return NETWORK_CONNECTION_END_FAILED;
		}

		m_socket->close();
		if (error)
		{
			return NETWORK_CONNECTION_END_FAILED;
		}

		return SUCCESS;
	}

} /* namespace havroc */
