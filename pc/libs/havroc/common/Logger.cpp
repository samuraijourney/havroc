#include <string>

#include <boost/thread/thread.hpp>
#include <havroc/common/Logger.h>

namespace havroc
{

	RemotePrintCallback Logger::m_remotePrintCallback = 0;
	bool				Logger::m_active = false;

	std::stringstream	Logger::log;

	void Logger::start_logger()
	{
		if (!m_active)
		{
			m_active = true;

			boost::thread(boost::bind(&Logger::logger_loop));
		}
	}

	void Logger::stop_logger()
	{
		m_active = false;
	}

	void Logger::logger_loop()
	{
		while (m_active)
		{
			if (!log.tellp())
			{
				std::string msg = log.str();

#if defined(PRINT_LOCAL)
				printf(msg.c_str());
#endif
#if defined(PRINT_REMOTE)
				if (m_remotePrintCallback)
				{
					m_remotePrintCallback(msg.c_str());
				}
			}
#endif
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}

}