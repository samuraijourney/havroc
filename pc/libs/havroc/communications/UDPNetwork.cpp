#include <iostream>

#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

UDPNetwork::UDPNetwork(boost::asio::io_service& service, int port, boost::shared_ptr<comm_signals_pack> signals_pack)
	: Network(service, signals_pack),
	  m_socket(service, udp::endpoint(udp::v4(), port)){}

int UDPNetwork::start_service()
{
	on_connect();

	return 0;
}

} /* namespace havroc */
