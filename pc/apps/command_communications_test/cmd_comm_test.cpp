#include <iostream>
#include <mutex>

#include <havroc/communications/NetworkManager.h>
#include <havroc/common/CommandManager.h>
#include <havroc/common/CommandBuilder.h>

#define NUM_MOTORS 24

std::mutex print_lock;

class TestTCPServerCallback
{
public:
	TestTCPServerCallback(){}
	~TestTCPServerCallback(){}

	void sent_callback(BYTE* msg, size_t size)
	{
		print_lock.lock();
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
		print_lock.unlock();
	}

	void receive_callback(BYTE* msg, size_t size)
	{
		print_lock.lock();
		if (havroc::CommandBuilder::is_command(msg, size))
		{
			std::cout << "TCP Server receiving packet" << std::endl;
		}
		else
		{
			std::string str_msg((char*) msg);
			std::cout << "TCP Server receiving message: " << str_msg << std::endl;
		}
		print_lock.unlock();
	}

	void connect_callback()
	{
		print_lock.lock();
		std::cout << "TCP Server started" << std::endl;
		print_lock.unlock();
	}

	void disconnect_callback()
	{
		print_lock.lock();
		std::cout << "TCP Server stopped" << std::endl;
		print_lock.unlock();
	}
};

class TestCommandCallback
{
public:
	TestCommandCallback(){}
	~TestCommandCallback(){}

	void tracking_callback(havroc::command_pkg* pkg)
	{
		BYTE* packet;
		size_t size;

		std::cout << "Tracking command received" << std::endl;
		havroc::CommandBuilder::build_command(packet, size, pkg);
		havroc::CommandBuilder::print_command(packet, size, 1);

		free(packet);
	}

	void system_callback(havroc::command_pkg* pkg)
	{
		BYTE* packet;
		size_t size;

		std::cout << "System command received" << std::endl;
		havroc::CommandBuilder::build_command(packet, size, pkg);
		havroc::CommandBuilder::print_command(packet, size, 1);

		free(packet);
	}

	void motor_callback(havroc::command_pkg* pkg)
	{
		BYTE* packet;
		size_t size;

		std::cout << "Motor command received" << std::endl;
		havroc::CommandBuilder::build_command(packet, size, pkg);
		havroc::CommandBuilder::print_command(packet, size, 1);

		free(packet);
	}
};

void connect_example_callback_with_no_class(){} // Do nothing
void tracking_example_callback_with_no_class(havroc::command_pkg* pkg){} // Do nothing

void error_callback(havroc::command_pkg* pkg)
{
	int error = (int)pkg->data[0];
	std::cout << "Unexpected error has occurred on device with code: " << error << std::endl;
}

int main()
{
	int port = -1;

	int choice;
	printf("Target:\t1) Board 1\n\t2) Board 2\nChoice: ");

	std::cin >> choice;

	switch (choice)
	{
	case(1) :
	{
		port = TCP_CLIENT_PORT_R;
		break;
	}
	case(2) :
	{
		port = TCP_CLIENT_PORT_L;
		break;
	}
	default:
	{
		printf("Invalid entry. Killing application.");
		return 0;
	}
	}

	havroc::NetworkManager* n_manager = havroc::NetworkManager::get();
	havroc::CommandManager* c_manager = havroc::CommandManager::get();

	n_manager->set_reconnect(true);

	TestTCPServerCallback* tcp = new TestTCPServerCallback();
	TestCommandCallback* cmd = new TestCommandCallback();

	n_manager->set_connections(TCP_SERVER);
	n_manager->register_connect_callback<TestTCPServerCallback>(&TestTCPServerCallback::connect_callback, tcp);
	n_manager->register_disconnect_callback<TestTCPServerCallback>(&TestTCPServerCallback::disconnect_callback, tcp);
	n_manager->register_sent_callback<TestTCPServerCallback>(&TestTCPServerCallback::sent_callback, tcp);
	n_manager->register_receive_callback<TestTCPServerCallback>(&TestTCPServerCallback::receive_callback, tcp);

	n_manager->register_connect_callback(&connect_example_callback_with_no_class);

	c_manager->register_tracking_callback<TestCommandCallback>(&TestCommandCallback::tracking_callback, cmd);
	c_manager->register_system_callback<TestCommandCallback>(&TestCommandCallback::system_callback, cmd);
	c_manager->register_motor_callback<TestCommandCallback>(&TestCommandCallback::motor_callback, cmd);

	c_manager->register_tracking_callback(&tracking_example_callback_with_no_class);
	c_manager->register_error_callback(&error_callback);

	n_manager->start_tcp_server(port);

	BYTE indices[NUM_MOTORS];
	BYTE intensities[NUM_MOTORS];

	while (n_manager->get_reconnect() || n_manager->is_active())
	{
		for (int i = 0; i < NUM_MOTORS; i++)
		{
			indices[i] = i;
			intensities[i] = rand() % 100 + 1;
		}

		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, true);
		if(int failures = n_manager->send(msg, size, true))
		{
			printf("Failed to send to %d connection(s)\n", failures);
		}

		havroc::CommandBuilder::build_kill_system_command(msg, size);
		if(int failures = n_manager->send(msg, size, true))
		{
			printf("Failed to send to %d connection(s)\n", failures);
		}

		havroc::CommandBuilder::build_motor_command(msg, size, indices, intensities, NUM_MOTORS);
		if(int failures = n_manager->send(msg, size, true))
		{
			printf("Failed to send to connection(s)\n", failures);
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}

	delete tcp;
	delete cmd;

	return 0;
}
