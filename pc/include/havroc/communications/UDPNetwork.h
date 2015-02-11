#ifndef UDPNETWORK_H_
#define UDPNETWORK_H_

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <havroc/communications/Network.h>

#define UDP_PORT 7999

using boost::asio::ip::udp;

namespace havroc
{
	class UDPNetwork : public Network
	{
	public:
		UDPNetwork(boost::asio::io_service& service, int port, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~UDPNetwork();

		virtual int start_service();

	protected:
		udp::socket m_socket;

	private:
		int kill_socket();
	};

	class UDPNetworkClient : public UDPNetwork
	{
	public:
		UDPNetworkClient(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~UDPNetworkClient();

		int  start_service();

	private:
		void receive();
		void handle_receive(const boost::system::error_code& error, std::size_t bytes);

		boost::array<char, 256> m_buffer;
	};

	class UDPNetworkServer : public UDPNetwork
	{
	public:
		UDPNetworkServer(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack = 0);
		virtual ~UDPNetworkServer();

		int broadcast(char* msg, size_t size, bool free_mem = false) { return send(msg, size, free_mem); }

	private:
		int  send(char* msg, size_t size, bool free_mem = false);
		void handle_send(char*, size_t, bool, const boost::system::error_code&, std::size_t);

		udp::endpoint m_broadcast_endpoint;
	};
} /* namespace havroc */

#endif /* UDPNETWORK_H_ */
