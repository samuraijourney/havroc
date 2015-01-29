#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/TCPNetwork.h>

bool _ip_received = false;
std::string _remote_ip = "";

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

void udp_receive_handler(std::string msg)
{
	std::cout << "UDP Client receiving: " << msg << std::endl;

	_remote_ip = msg;
	_ip_received = true;
}

void udp_stop_handler()
{
	std::cout << "UDP Client stopped" << std::endl;
}

void udp_start_handler()
{
	std::cout << "UDP Client started" << std::endl;
}

void tcp_sent_handler(std::string msg)
{
	std::cout << "TCP Client sent: " << msg;
}

void tcp_receive_handler(std::string msg)
{
	std::cout << "TCP Client receiving: " << msg;
}

void tcp_stop_handler()
{
	std::cout << "TCP Client stopped" << std::endl;
}

void tcp_start_handler()
{
	std::cout << "TCP Client started" << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		havroc::UDPNetworkClient udp(io_service);

		udp.get_receive_event().connect(&udp_receive_handler);
		udp.get_start_event().connect(&udp_start_handler);
		udp.get_stop_event().connect(&udp_stop_handler);

		udp.start_service();

		while (!_ip_received)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
		}

		udp.end_service();

		havroc::TCPNetworkClient tcp(io_service, _remote_ip);

		tcp.get_sent_event().connect(&tcp_sent_handler);
		tcp.get_receive_event().connect(&tcp_receive_handler);
		tcp.get_start_event().connect(&tcp_start_handler);
		tcp.get_stop_event().connect(&tcp_stop_handler);

		tcp.start_service();

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
