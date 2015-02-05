#include <iostream>

#include <havroc/communications/NetworkManager.h>
#include <havroc/common/CommandManager.h>
#include <havroc/common/CommandBuilder.h>

#define NUM_MOTORS 24

class TestTCPClientCallback
{
public:
	TestTCPClientCallback(){}
	~TestTCPClientCallback(){}

	void sent_callback(char* msg, size_t size)
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

	void receive_callback(char* msg, size_t size)
	{
		if (havroc::CommandBuilder::is_command(msg, size))
		{
			std::cout << "TCP Client receiving packet" << std::endl;
		}
		else
		{
			std::string str_msg(msg);
			std::cout << "TCP Client receiving message: " << str_msg << std::endl;
		}
	}

	void connect_callback()
	{
		std::cout << "TCP Client started" << std::endl;
	}

	void disconnect_callback()
	{
		std::cout << "TCP Client stopped" << std::endl;
	}
};

class TestCommandCallback
{
public:
	TestCommandCallback(){}
	~TestCommandCallback(){}

	void tracking_callback(havroc::command_pkg* pkg)
	{
		char* packet;
		size_t size;

		std::cout << "Tracking command received" << std::endl;
		havroc::CommandBuilder::build_command(packet, size, pkg);
		havroc::CommandBuilder::print_command(packet, size, 1);

		free(packet);
	}

	void system_callback(havroc::command_pkg* pkg)
	{
		char* packet;
		size_t size;

		std::cout << "System command received" << std::endl;
		havroc::CommandBuilder::build_command(packet, size, pkg);
		havroc::CommandBuilder::print_command(packet, size, 1);

		free(packet);
	}

	void motor_callback(havroc::command_pkg* pkg)
	{
		char* packet;
		size_t size;

		std::cout << "Motor command received" << std::endl;
		havroc::CommandBuilder::build_command(packet, size, pkg);
		havroc::CommandBuilder::print_command(packet, size, 1);

		free(packet);
	}
};

void connect_example_callback_with_no_class(){} // Do nothing
void tracking_example_callback_with_no_class(havroc::command_pkg* pkg){} // Do nothing

int main()
{
	havroc::NetworkManager* n_manager = havroc::NetworkManager::get();
	havroc::CommandManager* c_manager = havroc::CommandManager::get();

	n_manager->set_reconnect(true);

	TestTCPClientCallback* tcp = new TestTCPClientCallback();
	TestCommandCallback* cmd = new TestCommandCallback();

	n_manager->set_connections(TCP_CLIENT);
	n_manager->register_connect_callback<TestTCPClientCallback>(&TestTCPClientCallback::connect_callback, tcp);
	n_manager->register_disconnect_callback<TestTCPClientCallback>(&TestTCPClientCallback::disconnect_callback, tcp);
	n_manager->register_sent_callback<TestTCPClientCallback>(&TestTCPClientCallback::sent_callback, tcp);
	n_manager->register_receive_callback<TestTCPClientCallback>(&TestTCPClientCallback::receive_callback, tcp);

	n_manager->register_connect_callback(&connect_example_callback_with_no_class);

	c_manager->register_tracking_callback<TestCommandCallback>(&TestCommandCallback::tracking_callback, cmd);
	c_manager->register_system_callback<TestCommandCallback>(&TestCommandCallback::system_callback, cmd);
	c_manager->register_motor_callback<TestCommandCallback>(&TestCommandCallback::motor_callback, cmd);

	c_manager->register_tracking_callback(&tracking_example_callback_with_no_class);

	if(int error = n_manager->start_tcp_client("127.0.0.1"))
	{
		printf("TCP Client failed to start with error code: %d\n", error);
		printf("Terminating program\n");
		
		return -1;
	}

	char indices[NUM_MOTORS];
	char intensities[NUM_MOTORS];

	while (n_manager->get_reconnect() || n_manager->is_active())
	{
		for (int i = 0; i < NUM_MOTORS; i++)
		{
			indices[i] = i;
			intensities[i] = rand() % 100 + 1;
		}

		char* msg;
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
