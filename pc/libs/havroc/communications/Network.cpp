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

		m_signals_pack->connect_event.connect(boost::bind(&Network::init_loop, this));
	}

	Network::~Network(){}

	void Network::end_service()
	{
		if (m_active)
		{
			m_active = false;
			m_service.stop();

			kill_socket();

			on_disconnect();
		}
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
			m_service.poll();
			boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		}
	}

}
