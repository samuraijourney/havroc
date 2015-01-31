#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>

#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/CommandBuilder.h>

#define NUM_MOTORS 17

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

void sent_handler(char* msg, size_t size)
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

void disconnect_handler()
{
	std::cout << "UDP Server stopped" << std::endl;
}

void connect_handler()
{
	std::cout << "UDP Server started" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;
    havroc::UDPNetworkServer udp(io_service);

    udp.get_sent_event().connect(&sent_handler);
	udp.get_connect_event().connect(&connect_handler);
	udp.get_disconnect_event().connect(&disconnect_handler);

	udp.start_service();

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
		udp.broadcast(msg, size);

		havroc::CommandBuilder::build_kill_system_command(msg, size);
		udp.broadcast(msg, size);

		havroc::CommandBuilder::build_motor_command(msg, size, indices, intensities, NUM_MOTORS);
		udp.broadcast(msg, size);

		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}

	udp.end_service();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
