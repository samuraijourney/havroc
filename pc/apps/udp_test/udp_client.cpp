#include <iostream>

#include <boost/thread/thread.hpp>
#include <havroc/communications/UDPNetwork.h>
#include <havroc/common/CommandBuilder.h>

void receive_handler(BYTE* msg, size_t size)
{
	if (havroc::CommandBuilder::is_command(msg, size))
	{
		std::cout << "UDP Client receiving packet" << std::endl;
		havroc::CommandBuilder::print_command(msg, size, 1);
	}
	else
	{
		std::string str_msg((char*) msg);
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

	udp.register_receive_callback(&receive_handler);
	udp.register_connect_callback(&connect_handler);
	udp.register_disconnect_callback(&disconnect_handler);

    udp.start_service();

    while(udp.is_active())
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
