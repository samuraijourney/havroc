#ifndef TCPNETWORK_H_
#define TCPNETWORK_H_

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include <havroc/communications/Network.h>

#define DEFAULT_TCP_PORT 13

using boost::asio::ip::tcp;

namespace havroc {

	class TCPNetwork : public Network
	{
	public:
		TCPNetwork(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~TCPNetwork();

		int	 send(BYTE* msg, size_t size, bool free_mem = false);

		int	 get_port() { return m_port; }

		bool is_connecting() { return m_connecting; }

		virtual int start_service() = 0;

	protected:
		void handle_accept(const boost::system::error_code& ec);

		tcp::socket m_socket;
		tcp::endpoint m_endpoint;

		int m_port;
		bool m_connecting;

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error, std::size_t bytes);
		void handle_send(BYTE*, size_t, bool, const boost::system::error_code&, std::size_t);
		int  kill_socket();

		boost::array<char, 49152> m_buffer; // 48 kb
	};

	class TCPNetworkClient : public TCPNetwork
	{
	public:
		TCPNetworkClient(boost::asio::io_service& service, std::string ip, int port, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		TCPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~TCPNetworkClient();

		int			set_connection(std::string ip, int port);

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

		int set_port(int port);

		int start_service();

	private:
		tcp::acceptor m_acceptor;
	};
} /* namespace havroc */

#endif /* TCPNETWORK_H_ */
