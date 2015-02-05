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
			return -1;
		}

		on_connect();

		return 0;
	}

	void UDPNetwork::kill_socket()
	{
		m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
		m_socket.close();
	}

} /* namespace havroc */
