#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/CommandBuilder.h>

#define NUM_MOTORS 50

bool _tcp_connected = false;

void udp_sent_handler(BYTE* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "UDP Server sent packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
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

void tcp_sent_handler(BYTE* msg, size_t size)
{

	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Server sent packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
		std::cout << "TCP Server sent message: " << str_msg << std::endl;
	}
}

void tcp_receive_handler(BYTE* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Server receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
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
	BYTE* ip = (BYTE*)malloc(sizeof(BYTE) * 30);
	bool free_mem = false;
	size_t size = 0;

	int choice;
	printf("Broadcast IP:\t1) Local\n\t\t2) CC3200\n\t\t3) Custom IP\nChoice: ");

	std::cin >> choice;

	switch (choice)
	{
		case(1) :
		{
			free(ip);
			ip = (BYTE*)"127.0.0.1";
			size = 9;
			break;
		}
		case(2) :
		{
			free(ip);
			ip = (BYTE*)CC3200_IP;
			size = 13;
			break;
		}
		case(3) :
		{
			printf("Enter IP: ");
			std::cin >> ip;
			printf("Enter character length of IP: ");
			std::cin >> size;
			free_mem = true;
			break;
		}
		default:
		{
			printf("Invalid entry. Killing application.");
			return 0;
		}
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

		if (free_mem)
		{
			free(ip);
		}

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
