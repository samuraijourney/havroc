#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/CommandBuilder.h>

#define NUM_MOTORS 32

void sent_handler(BYTE* msg, size_t size)
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

void receive_handler(BYTE* msg, size_t size)
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

void disconnect_handler()
{
	std::cout << "TCP Server stopped" << std::endl;
}

void connect_handler()
{
	std::cout << "TCP Server started" << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		havroc::TCPNetworkServer tcp(io_service);

		tcp.get_sent_event().connect(&sent_handler);
		tcp.get_receive_event().connect(&receive_handler);
		tcp.get_connect_event().connect(&connect_handler);
		tcp.get_disconnect_event().connect(&disconnect_handler);

		std::cout << "Listening on port: " << TCP_PORT << std::endl;
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
