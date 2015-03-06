#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/CommandBuilder.h>

#define NUM_MOTORS 24

void sent_handler(BYTE* msg, size_t size)
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

void receive_handler(BYTE* msg, size_t size)
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

		tcp.register_sent_callback(&sent_handler);
		tcp.register_receive_callback(&receive_handler);
		tcp.register_connect_callback(&connect_handler);
		tcp.register_disconnect_callback(&disconnect_handler);

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
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
