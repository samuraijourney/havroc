#include <iostream>

#include <ctime>
#include <boost/thread/thread.hpp>
#include <havroc/communications/TCPNetwork.h>

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

void sent_handler(std::string msg)
{
	std::cout << "TCP Client sent: " << msg;
}

void receive_handler(std::string msg)
{
	std::cout << "TCP Client receiving: " << msg;
}

void stop_handler()
{
	std::cout << "TCP Client stopped" << std::endl;
}

void start_handler()
{
	std::cout << "TCP Client started" << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		havroc::TCPNetworkClient* tcp = new havroc::TCPNetworkClient(io_service,"127.0.0.1");

		tcp->get_sent_event().connect(&sent_handler);
		tcp->get_receive_event().connect(&receive_handler);
		tcp->get_start_event().connect(&start_handler);
		tcp->get_stop_event().connect(&stop_handler);

		tcp->start_service();

		while (true)
		{
			tcp->send(make_daytime_string());

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		tcp->end_service();

		delete tcp;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
