#include <havroc/communications/Network.h>

namespace havroc
{

	Network::Network(boost::asio::io_service& service, boost::shared_ptr<comm_signals_pack> signals_pack)
	: m_service(service),
	  m_active(false)
	{
		if (signals_pack == 0)
		{
			signals_pack = boost::shared_ptr<comm_signals_pack>(new comm_signals_pack());
		}

		m_signals_pack = signals_pack;

		register_connect_callback<Network>(&Network::init_loop, this);
	}

	Network::~Network(){}

	int Network::end_service(int error)
	{
		if (m_active)
		{
			m_active = false;
			m_service.stop();

			if (int kill_error = kill_socket())
			{
				error = kill_error;
			}

			if (error != SUCCESS)
			{
				printf("Irregular termination of network service has occurred with error code: %d\n", error);
			}

			on_disconnect();

			return error;
		}

		return NETWORK_IS_INACTIVE;
	}

	void Network::init_loop()
	{
		m_active = true;

		m_poll_thread = boost::thread(boost::bind(&Network::loop, this));
	}

	void Network::loop()
	{
		while(m_active)
		{
			boost::system::error_code error;

			m_service.poll(error);
			if (error)
			{
				end_service(NETWORK_UNEXPECTED_EVENT_LOOP_FAILURE);
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		}
	}

}
