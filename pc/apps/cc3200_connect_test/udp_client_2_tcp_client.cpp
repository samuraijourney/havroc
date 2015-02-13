#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/CommandBuilder.h>

#define NUM_MOTORS 12

bool _ip_received = false;
std::string _remote_ip = "";

void udp_receive_handler(BYTE* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "UDP Client receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
		std::cout << "UDP Client receiving message: " << str_msg << std::endl;
	}

	_remote_ip = (char*) msg;
	_ip_received = true;
}

void udp_disconnect_handler()
{
	std::cout << "UDP Client stopped" << std::endl;
}

void udp_connect_handler()
{
	std::cout << "UDP Client started" << std::endl;
}

void tcp_sent_handler(BYTE* msg, size_t size)
{

	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Client sent packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
		std::cout << "TCP Client sent message: " << str_msg << std::endl;
	}
}

void tcp_receive_handler(BYTE* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Client receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
		std::cout << "TCP Client receiving message: " << str_msg << std::endl;
	}
}

void tcp_disconnect_handler()
{
	std::cout << "TCP Client stopped" << std::endl;
}

void tcp_connect_handler()
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
		udp.get_connect_event().connect(&udp_connect_handler);
		udp.get_disconnect_event().connect(&udp_disconnect_handler);

		udp.start_service();

		while (!_ip_received)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
		}

		udp.end_service();

		havroc::TCPNetworkClient tcp(io_service, _remote_ip);

		tcp.get_sent_event().connect(&tcp_sent_handler);
		tcp.get_receive_event().connect(&tcp_receive_handler);
		tcp.get_connect_event().connect(&tcp_connect_handler);
		tcp.get_disconnect_event().connect(&tcp_disconnect_handler);

		tcp.start_service();

		BYTE indices[NUM_MOTORS];
		BYTE intensities[NUM_MOTORS];

		while (tcp.is_active())
		{
			for (int i = 0; i < NUM_MOTORS; i++)
			{
				indices[i] = i;
				intensities[i] = rand() % 100 + 1;
			}

			BYTE* msg;
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
