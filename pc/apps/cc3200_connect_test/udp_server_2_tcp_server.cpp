#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/TCPNetwork.h>

bool _tcp_connected = false;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

void udp_sent_handler(std::string msg)
{
	std::cout << "UDP Server sent: " << msg << std::endl;
}

void udp_stop_handler()
{
	std::cout << "UDP Server stopped" << std::endl;
}

void udp_start_handler()
{
	std::cout << "UDP Server started" << std::endl;
}

void tcp_sent_handler(std::string msg)
{
	std::cout << "TCP Server sent: " << msg;
}

void tcp_receive_handler(std::string msg)
{
	std::cout << "TCP Server receiving: " << msg;
}

void tcp_stop_handler()
{
	std::cout << "TCP Server stopped" << std::endl;
}

void tcp_start_handler()
{
	std::cout << "TCP Server started" << std::endl;

	_tcp_connected = true;
}

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		havroc::UDPNetworkServer udp(io_service);

		udp.get_sent_event().connect(&udp_sent_handler);
		udp.get_start_event().connect(&udp_start_handler);
		udp.get_stop_event().connect(&udp_stop_handler);

		udp.start_service();

		havroc::TCPNetworkServer tcp(io_service);

		tcp.get_sent_event().connect(&tcp_sent_handler);
		tcp.get_receive_event().connect(&tcp_receive_handler);
		tcp.get_start_event().connect(&tcp_start_handler);
		tcp.get_stop_event().connect(&tcp_stop_handler);

		boost::thread(boost::bind(&havroc::TCPNetworkServer::start_service, &tcp));

		while (!_tcp_connected)
		{
			udp.broadcast("127.0.0.1");

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		udp.end_service();

		while (true)
		{
			tcp.send(make_daytime_string());

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		tcp.end_service();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
