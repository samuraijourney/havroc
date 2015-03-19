#include <iostream>

#include <mutex>
#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/NetworkManager.h>
#include <havroc/communications/TCPNetwork.h>
#include <havroc/common/CommandBuilder.h>
#include <havroc/common/Logger.h>

#define NUM_MOTORS 24

std::mutex print_lock;

void sent_handler(BYTE* msg, size_t size)
{
	print_lock.lock();
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
	print_lock.unlock();
}

void receive_handler(BYTE* msg, size_t size)
{
	print_lock.lock();
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
	print_lock.unlock();
}

void disconnect_handler()
{
	print_lock.lock();
	std::cout << "TCP Client stopped" << std::endl;
	print_lock.unlock();
}

void connect_handler()
{
	print_lock.lock();
	std::cout << "TCP Client started" << std::endl;
	print_lock.unlock();
}

int main(int argc, char* argv[])
{
	std::string ip = "127.0.0.1";

	int choice;
	printf("Target:\t1) Local\n\t2) CC3200\n\t3) Custom IP\nChoice: ");

	std::cin >> choice;

	switch (choice)
	{
	case(1) :
	{
		break;
	}
	case(2) :
	{
		ip = "";
		break;
	}
	case(3) :
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
		havroc::NetworkManager* manager = havroc::NetworkManager::get();
		manager->set_reconnect(true);

		manager->set_connections(TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT);
		manager->register_connect_callback(&connect_handler);
		manager->register_disconnect_callback(&disconnect_handler);
		manager->register_sent_callback(&sent_handler);
		manager->register_receive_callback(&receive_handler);

		havroc::Logger::start_logger();

		char* c_ip = (char*)malloc(sizeof(char)*(ip.length() + 1));
		strcpy(c_ip, ip.c_str());

		if (ip == "")
		{
			manager->async_start_tcp_client_left();
			manager->async_start_tcp_client_right();
		}
		else
		{
			manager->async_start_tcp_client_left(c_ip);
			manager->async_start_tcp_client_right(c_ip);
		}

		while (!manager->is_tcp_client_left_active() || !manager->is_tcp_client_right_active())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		BYTE indices[NUM_MOTORS];
		BYTE intensities[NUM_MOTORS];

		while (true)
		{
			for (int i = 0; i < NUM_MOTORS; i++)
			{
				indices[i] = i;
				intensities[i] = rand() % 100 + 1;
			}

			BYTE* msg;
			size_t size;

			std::cout << "Press enter to send command" << std::endl;
			std::cin.get();

			havroc::CommandBuilder::build_tracking_command(msg, size, true);
			manager->send(msg, size, true);

			std::cout << "Press enter to send command" << std::endl;
			std::cin.get();

			havroc::CommandBuilder::build_kill_system_command(msg, size);
			manager->send(msg, size, true);

			std::cout << "Press enter to send command" << std::endl;
			std::cin.get();

			havroc::CommandBuilder::build_motor_command(msg, size, indices, intensities, NUM_MOTORS);
			manager->send(msg, size, true);

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		free(c_ip);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
