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

		virtual int start_service();

	protected:
		udp::socket m_socket;

	private:
		void kill_socket()
		{
			m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
			m_socket.close();
		}
	};

	class UDPNetworkClient : public UDPNetwork
	{
	public:
		UDPNetworkClient(boost::asio::io_service& service) : UDPNetwork(service, UDP_PORT){}
		virtual ~UDPNetworkClient(){}

		int  start_service();

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error,
							std::size_t bytes);

		boost::array<char, 256> m_buffer;
	};

	class UDPNetworkServer : public UDPNetwork
	{
	public:
		UDPNetworkServer(boost::asio::io_service& service) : UDPNetwork(service, 0)
		{
			m_socket.set_option(boost::asio::socket_base::broadcast(true));
			m_broadcast_endpoint = udp::endpoint(boost::asio::ip::address_v4::broadcast(), UDP_PORT);
		}
		virtual ~UDPNetworkServer() {}

		int broadcast(char* msg, size_t size) { return send(msg, size); }

	private:
		int  send(char* msg, size_t size);
		void handle_send(char*,
						 size_t,
						 const boost::system::error_code&,
						 std::size_t);

		udp::endpoint m_broadcast_endpoint;
	};
} /* namespace havroc */

#endif /* UDPNETWORK_H_ */
