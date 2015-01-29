/*
 * UDPNetwork.h
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

#ifndef UDPNETWORK_H_
#define UDPNETWORK_H_

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Network.h"

#define UDP_PORT 8888

using boost::asio::ip::udp;

namespace havroc
{
	class UDPNetwork : public Network
	{
	public:
		UDPNetwork(boost::asio::io_service& service, int port);
		virtual ~UDPNetwork(){}

		int  broadcast(std::string msg);

		int  start_service();
		void end_service();

	protected:
		udp::socket m_socket;

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error,
						    std::size_t bytes);

		int  send(std::string msg);
		void handle_send(boost::shared_ptr<std::string>,
						 const boost::system::error_code&,
						 std::size_t);

		udp::endpoint m_broadcast_endpoint;

		boost::array<char,256> m_buffer;
	};

	class UDPNetworkClient : public UDPNetwork
	{
	public:
		UDPNetworkClient(boost::asio::io_service& service) : UDPNetwork(service, UDP_PORT){}
		virtual ~UDPNetworkClient(){}
	};

	class UDPNetworkServer : public UDPNetwork
	{
	public:
		UDPNetworkServer(boost::asio::io_service& service) : UDPNetwork(service,0)
		{
			m_socket.set_option(boost::asio::socket_base::broadcast(true));
		}
		virtual ~UDPNetworkServer() {}
	};
} /* namespace havroc */

#endif /* UDPNETWORK_H_ */
