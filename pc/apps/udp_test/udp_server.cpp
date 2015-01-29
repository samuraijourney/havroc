#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>
#include <havroc/communications/UDPNetwork.h>

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

void receive_handler(std::string msg)
{
	std::cout << "Server receiving: " << msg << std::endl;
}

void sent_handler(std::string msg)
{
	std::cout << "Server sending: " << msg << std::endl;
}

void stop_handler()
{
	std::cout << "Server stopped" << std::endl;
}

void start_handler()
{
	std::cout << "Server started" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;
    havroc::UDPNetworkServer* udp = new havroc::UDPNetworkServer(io_service);

    udp->get_receive_event().connect(&receive_handler);
    udp->get_sent_event().connect(&sent_handler);
    udp->get_start_event().connect(&start_handler);
    udp->get_stop_event().connect(&stop_handler);

    udp->start_service();

    while(true)
    {
    	udp->broadcast(make_daytime_string());

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
