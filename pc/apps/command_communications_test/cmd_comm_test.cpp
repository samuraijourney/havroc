#include <iostream>

#include <havroc/communications/NetworkManager.h>
#include <havroc/communications/CommandBuilder.h>

class TestTCPServerCallback
{
public:
	TestTCPServerCallback(){}
	~TestTCPServerCallback(){}

	void sent_callback(char* msg, size_t size)
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

	void receive_callback(char* msg, size_t size)
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

	void connect_callback()
	{
		std::cout << "TCP Server started" << std::endl;
	}

	void disconnect_callback()
	{
		std::cout << "TCP Server stopped" << std::endl;
	}
};

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
			havroc::CommandBuilder::print_command(msg, size, 1);
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

int main()
{
	havroc::NetworkManager* manager = havroc::NetworkManager::get();

	TestTCPClientCallback* tcp_client = new TestTCPClientCallback();

	manager->set_desired_connections(TCP_CLIENT);
	manager->register_connect_callback<TestTCPClientCallback>(&TestTCPClientCallback::connect_callback, tcp_client);
	manager->register_disconnect_callback<TestTCPClientCallback>(&TestTCPClientCallback::disconnect_callback, tcp_client);
	manager->register_sent_callback<TestTCPClientCallback>(&TestTCPClientCallback::sent_callback, tcp_client);
	manager->register_receive_callback<TestTCPClientCallback>(&TestTCPClientCallback::receive_callback, tcp_client);

	TestTCPServerCallback* tcp_server = new TestTCPServerCallback();

	manager->set_desired_connections(TCP_SERVER);
	manager->register_connect_callback<TestTCPServerCallback>(&TestTCPServerCallback::connect_callback, tcp_server);
	manager->register_disconnect_callback<TestTCPServerCallback>(&TestTCPServerCallback::disconnect_callback, tcp_server);
	manager->register_sent_callback<TestTCPServerCallback>(&TestTCPServerCallback::sent_callback, tcp_server);
	manager->register_receive_callback<TestTCPServerCallback>(&TestTCPServerCallback::receive_callback, tcp_server);

	manager->start_tcp_server();

	while (true)
	{
		manager->send("Hello Akram");
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}

	return 0;
}
