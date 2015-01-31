#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/TCPNetwork.h>
#include <havroc/communications/CommandBuilder.h>

#define NUM_MOTORS 50

bool _tcp_connected = false;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

void udp_sent_handler(char* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "UDP Server sent packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg(msg);
		std::cout << "UDP Server sent message: " << str_msg << std::endl;
	}
}

void udp_disconnect_handler()
{
	std::cout << "UDP Server stopped" << std::endl;
}

void udp_connect_handler()
{
	std::cout << "UDP Server started" << std::endl;
}

void tcp_sent_handler(char* msg, size_t size)
{

	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Server sent packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg(msg);
		std::cout << "TCP Server sent message: " << str_msg << std::endl;
	}
}

void tcp_receive_handler(char* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Server receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg(msg);
		std::cout << "TCP Server receiving message: " << str_msg << std::endl;
	}
}

void tcp_disconnect_handler()
{
	std::cout << "TCP Server stopped" << std::endl;
}

void tcp_connect_handler()
{
	std::cout << "TCP Server started" << std::endl;

	_tcp_connected = true;
}

int main(int argc, char* argv[])
{
	char* ip = "127.0.0.1";
	size_t size = 9;

	if (argc == 3)
	{
		ip = argv[1];
		size = atoi(argv[2]);
	}

	try
	{
		boost::asio::io_service io_service;
		havroc::UDPNetworkServer udp(io_service);

		udp.get_sent_event().connect(&udp_sent_handler);
		udp.get_connect_event().connect(&udp_connect_handler);
		udp.get_disconnect_event().connect(&udp_disconnect_handler);

		udp.start_service();

		havroc::TCPNetworkServer tcp(io_service);

		tcp.get_sent_event().connect(&tcp_sent_handler);
		tcp.get_receive_event().connect(&tcp_receive_handler);
		tcp.get_connect_event().connect(&tcp_connect_handler);
		tcp.get_disconnect_event().connect(&tcp_disconnect_handler);

		boost::thread(boost::bind(&havroc::TCPNetworkServer::start_service, &tcp));

		while (!_tcp_connected)
		{
			udp.broadcast(ip, size);

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		udp.end_service();

		char indices[NUM_MOTORS];
		char intensities[NUM_MOTORS];

		while (true)
		{
			for (int i = 0; i < NUM_MOTORS; i++)
			{
				indices[i] = i;
				intensities[i] = rand() % 100 + 1;
			}

			char* msg;
			size_t size;

			havroc::CommandBuilder::build_tracking_command(msg, size, true);
			tcp.send(msg, size);

			havroc::CommandBuilder::build_kill_system_command(msg, size);
			tcp.send(msg, size);

			havroc::CommandBuilder::build_motor_command(msg, size, indices, intensities, NUM_MOTORS);
			tcp.send(msg, size);

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
