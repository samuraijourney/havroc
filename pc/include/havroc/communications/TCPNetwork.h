/*
 * TCPNetwork.h
 *
 *  Created on: Jan 26, 2015
 *      Author: Akram
 */

#ifndef TCPNETWORK_H_
#define TCPNETWORK_H_

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Network.h"

#define TCP_PORT 13

using boost::asio::ip::tcp;

namespace havroc {

	class TCPNetwork : public Network
	{
	public:
		TCPNetwork(boost::asio::io_service& service) : Network(service), m_socket(service){}
		virtual ~TCPNetwork(){}

		int  send(std::string msg);

		virtual int start_service() = 0;
		void end_service();

	protected:
		void handle_accept(const boost::system::error_code& ec);

		tcp::socket m_socket;
		tcp::endpoint m_endpoint;

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error,
							std::size_t bytes);
		void handle_send(boost::shared_ptr<std::string>,
						 const boost::system::error_code&,
						 std::size_t);

		boost::array<char,256> m_buffer;
	};

	class TCPNetworkClient : public TCPNetwork
	{
	public:
		TCPNetworkClient(boost::asio::io_service& service, std::string ip) : TCPNetwork(service), m_ip(ip){}
		virtual ~TCPNetworkClient(){}

		int start_service()
		{
		    m_endpoint = tcp::endpoint(boost::asio::ip::address::from_string(m_ip), TCP_PORT);

		    m_socket.get_io_service().run();

		    m_socket.async_connect(m_endpoint, boost::bind(&TCPNetworkClient::handle_accept, this,
		    		boost::asio::placeholders::error));
		}

	private:
		std::string m_ip;
	};

	class TCPNetworkServer : public TCPNetwork
	{
	public:
		TCPNetworkServer(boost::asio::io_service& service)
		: TCPNetwork(service), m_acceptor(service, tcp::endpoint(tcp::v4(), TCP_PORT)){}
		virtual ~TCPNetworkServer(){}

		int start_service()
		{
			m_socket.get_io_service().run();

			m_acceptor.async_accept(m_socket,
				boost::bind(&TCPNetworkServer::handle_accept, this,
				boost::asio::placeholders::error));
		}

	private:
		tcp::acceptor m_acceptor;
	};
} /* namespace havroc */

#endif /* TCPNETWORK_H_ */
