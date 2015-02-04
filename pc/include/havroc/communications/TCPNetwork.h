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
#include <boost/thread/thread.hpp>

#include "Network.h"

#define TCP_PORT 13

using boost::asio::ip::tcp;

namespace havroc {

	class TCPNetwork : public Network
	{
	public:
		TCPNetwork(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0) 
			: Network(service, signals_pack), m_socket(service){}
		virtual ~TCPNetwork(){}

		int	 send(char* msg, size_t size, bool free_mem = false);

		virtual int start_service() = 0;

	protected:
		void handle_accept(const boost::system::error_code& ec);

		tcp::socket m_socket;
		tcp::endpoint m_endpoint;

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error,
							std::size_t bytes);
		void handle_send(char*,
						 size_t,
						 bool,
						 const boost::system::error_code&,
						 std::size_t);
		void kill_socket()
		{
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			m_socket.close();
		}

		boost::array<char,256> m_buffer;
	};

	class TCPNetworkClient : public TCPNetwork
	{
	public:
		TCPNetworkClient(boost::asio::io_service& service, std::string ip, boost::shared_ptr<comm_signals_pack> signals_pack = 0) 
			: TCPNetwork(service, signals_pack)
		{
			set_ip(ip);
		}
		TCPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0) 
			: TCPNetwork(service, signals_pack)
		{
			set_ip(CC3200_IP);
		}
		virtual ~TCPNetworkClient(){}

		void set_ip(std::string ip)
		{
			if (!is_active())
			{
				m_ip = ip;
				m_endpoint = tcp::endpoint(boost::asio::ip::address::from_string(m_ip), TCP_PORT);
			}
		}

		std::string get_ip() { return m_ip; }

		int start_service()
		{
			printf("\n\nTCP Client waiting for connection...\n\n");
			int handles = 1;

			m_socket.get_io_service().reset();

			while (!is_active())
			{
				if (handles == 1)
				{
					m_socket.async_connect(m_endpoint, boost::bind(&TCPNetworkClient::handle_accept, this,
						boost::asio::placeholders::error));
				}

				handles = m_socket.get_io_service().poll();
				boost::this_thread::sleep(boost::posix_time::milliseconds(50));
			}

			return 0;
		}

	private:
		std::string m_ip;
	};

	class TCPNetworkServer : public TCPNetwork
	{
	public:
		TCPNetworkServer(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0)
			: TCPNetwork(service, signals_pack), m_acceptor(service, tcp::endpoint(tcp::v4(), TCP_PORT)){}
		virtual ~TCPNetworkServer(){}

		int start_service()
		{
			printf("\n\nTCP Server waiting for connection...\n\n");

			int handles = 1;

			m_socket.get_io_service().reset();

			while (!is_active())
			{
				if (handles == 1)
				{
					m_acceptor.async_accept(m_socket,
						boost::bind(&TCPNetworkServer::handle_accept, this,
						boost::asio::placeholders::error));
				}

				handles = m_socket.get_io_service().poll();
				boost::this_thread::sleep(boost::posix_time::milliseconds(50));
			}

			return 0;
		}

	private:
		tcp::acceptor m_acceptor;
	};
} /* namespace havroc */

#endif /* TCPNETWORK_H_ */
