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
		TCPNetwork(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~TCPNetwork();

		int	 send(char* msg, size_t size, bool free_mem = false);

		virtual int start_service() = 0;

	protected:
		void handle_accept(const boost::system::error_code& ec);

		tcp::socket m_socket;
		tcp::endpoint m_endpoint;

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error, std::size_t bytes);
		void handle_send(char*, size_t, bool, const boost::system::error_code&, std::size_t);
		void kill_socket();

		boost::array<char,256> m_buffer;
	};

	class TCPNetworkClient : public TCPNetwork
	{
	public:
		TCPNetworkClient(boost::asio::io_service& service, std::string ip, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		TCPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~TCPNetworkClient();

		void		set_ip(std::string ip);
		std::string get_ip() { return m_ip; }

		int start_service();

	private:
		std::string m_ip;
	};

	class TCPNetworkServer : public TCPNetwork
	{
	public:
		TCPNetworkServer(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~TCPNetworkServer();

		int start_service();

	private:
		tcp::acceptor m_acceptor;
	};
} /* namespace havroc */

#endif /* TCPNETWORK_H_ */
