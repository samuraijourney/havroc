#include <havroc/communications/UDPNetwork.h>

namespace havroc
{

	UDPNetworkServer::UDPNetworkServer(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
		: UDPNetwork(service, signals_pack)
	{
		
		m_broadcast_endpoint = udp::endpoint(boost::asio::ip::address_v4::broadcast(), UDP_PORT);
	}

	UDPNetworkServer::~UDPNetworkServer(){}

	int UDPNetworkServer::start_service()
	{
		if (is_active())
		{
			return NETWORK_IS_ACTIVE;
		}

		m_socket = boost::shared_ptr<udp::socket>(new udp::socket(m_service, udp::endpoint(udp::v4(), 0)));
		m_socket->set_option(boost::asio::socket_base::broadcast(true));

		on_connect();

		return SUCCESS;
	}

	int UDPNetworkServer::send(BYTE* msg, size_t size, bool free_mem)
	{
		if (is_active())
		{
			m_socket->async_send_to(boost::asio::buffer(msg,size), m_broadcast_endpoint,
				boost::bind(&UDPNetworkServer::handle_send, this, msg, size, free_mem,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
			
			return SUCCESS;
		}

		return NETWORK_IS_INACTIVE;
	}

	void UDPNetworkServer::handle_send(BYTE* msg /*message*/,
									   size_t size /*message size*/,
									   bool free_mem /*ownership*/,
									   const boost::system::error_code& /*error*/,
									   std::size_t /*bytes_transferred*/)
	{
		on_sent(msg, size, free_mem);
	}

} /* namespace havroc */
