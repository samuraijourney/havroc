#include <iostream>

#include <boost/thread/thread.hpp>
#include <havroc/communications/UDPNetwork.h>
#include <havroc/communications/CommandBuilder.h>

void receive_handler(char* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "UDP Client receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg(msg);
		std::cout << "UDP Client receiving message: " << str_msg << std::endl;
	}
}

void disconnect_handler()
{
	std::cout << "UDP Client stopped" << std::endl;
}

void connect_handler()
{
	std::cout << "UDP Client started" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;
    havroc::UDPNetworkClient udp(io_service);

    udp.get_receive_event().connect(&receive_handler);
	udp.get_connect_event().connect(&connect_handler);
	udp.get_disconnect_event().connect(&disconnect_handler);

    udp.start_service();

    while(true)
    {
    	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    }

    udp.end_service();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
