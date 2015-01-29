#include <iostream>

#include <boost/thread/thread.hpp>
#include <havroc/communications/UDPNetwork.h>

void receive_handler(std::string msg)
{
	std::cout << "UDP Client receiving: " << msg;
}

void stop_handler()
{
	std::cout << "UDP Client stopped" << std::endl;
}

void start_handler()
{
	std::cout << "UDP Client started" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;
    havroc::UDPNetworkClient* udp = new havroc::UDPNetworkClient(io_service);

    udp->get_receive_event().connect(&receive_handler);
    udp->get_start_event().connect(&start_handler);
    udp->get_stop_event().connect(&stop_handler);

    udp->start_service();

    while(true)
    {
    	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    }

    udp->end_service();

    delete udp;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
