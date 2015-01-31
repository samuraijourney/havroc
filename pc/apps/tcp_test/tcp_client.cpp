#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/TCPNetwork.h>
#include <havroc/communications/CommandBuilder.h>

#define NUM_MOTORS 24

std::string make_daytime_string()
{
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}

void sent_handler(char* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Client sent packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg(msg);
		std::cout << "TCP Client sent message: " << str_msg << std::endl;
	}
}

void receive_handler(char* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "TCP Client receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg(msg);
		std::cout << "TCP Client receiving message: " << str_msg << std::endl;
	}
}

void disconnect_handler()
{
	std::cout << "TCP Client stopped" << std::endl;
}

void connect_handler()
{
	std::cout << "TCP Client started" << std::endl;
}

int main(int argc, char* argv[])
{
	std::string ip = "127.0.0.1";

	int choice;
	printf("Target:\t1) Local\n\t2) CC3200\n\t3) Custom IP\nChoice: ");

	std::cin >> choice;

	switch (choice)
	{
		case(1):
		{
			break;
		}
		case(2):
		{
			ip = CC3200_IP;
			break;
		}
		case(3):
		{
			printf("Enter IP: ");
			std::cin >> ip;
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
		havroc::TCPNetworkClient tcp(io_service, ip);

		tcp.get_sent_event().connect(&sent_handler);
		tcp.get_receive_event().connect(&receive_handler);
		tcp.get_connect_event().connect(&connect_handler);
		tcp.get_disconnect_event().connect(&disconnect_handler);

		tcp.start_service();
		tcp.set_reconnect(true);

		char indices[NUM_MOTORS];
		char intensities[NUM_MOTORS];

		while (tcp.is_active())
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
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
