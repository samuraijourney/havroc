#include <string>

#include <boost/thread/thread.hpp>
#include <havroc/common/Logger.h>

namespace havroc
{
	RemotePrintCallback Logger::m_remote_print_callback = 0;

#ifndef NO_LOGGER

#if defined(PRINT_LOCAL)
	void Logger::log(const char *format, ...)
	{
		va_list arg;

		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);
	}

#elif defined(PRINT_REMOTE)
	void Logger::log(const char *format, ...)
	{
		if (!m_remote_print_callback)
		{
			return;
		}

		va_list arg;

		char* temp = (char*)malloc(LOG_STRING_LIMIT_IN_BYTES);

		va_start(arg, format);
		vsprintf(temp, format, arg);
		va_end(arg);

		std::string tempStr = temp;

		m_remote_print_callback(tempStr.c_str());

		free(temp);
	}

#elif defined(PRINT_LOCAL_AND_REMOTE)
	void Logger::log(const char *format, ...)
	{
		va_list arg;

		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);

		if (!m_remote_print_callback)
		{
			return;
		}

		char* temp = (char*)malloc(LOG_STRING_LIMIT_IN_BYTES);

		va_start(arg, format);
		vsprintf(temp, format, arg);
		va_end(arg);

		std::string tempStr = temp;

		m_remote_print_callback(tempStr.c_str());

		free(temp);
	}
#endif

#endif
}