#include <string>

#include <boost/thread/thread.hpp>
#include <havroc/common/Logger.h>

namespace havroc
{
	RemotePrintCallback Logger::m_remote_print_callback = 0;

#ifndef NO_LOGGER

#if defined(PRINT_LOCAL)
	void Logger::log(const char* file_name, int line_number, const char *format, ...)
	{
		va_list arg;

		printf("%s:%d ", file_name, line_number);
		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);
	}

#elif defined(PRINT_REMOTE)
	void Logger::log(const char* file_name, int line_number, const char *format, ...)
	{
		if (!m_remote_print_callback)
		{
			return;
		}

		va_list arg;

		char* temp = (char*)malloc(LOG_STRING_LIMIT_IN_BYTES);
		sprintf(temp, "%s:%d ", file_name, line_number);
		int length = strlen(std::string(temp).c_str());

		va_start(arg, format);
		vsprintf(&temp[length], format, arg);
		va_end(arg);

		std::string tempStr = temp;

		m_remote_print_callback(tempStr.c_str());

		free(temp);
	}

#elif defined(PRINT_LOCAL_AND_REMOTE)
	void Logger::log(const char* file_name, int line_number, const char *format, ...)
	{
		va_list arg;

		printf("%s:%d ", file_name, line_number);
		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);

		if (!m_remote_print_callback)
		{
			return;
		}

		char* temp = (char*)malloc(LOG_STRING_LIMIT_IN_BYTES);
		sprintf(temp, "%s:%d ", file_name, line_number);
		int length = strlen(std::string(temp).c_str());

		va_start(arg, format);
		vsprintf(&temp[length], format, arg);
		va_end(arg);

		std::string tempStr = temp;

		m_remote_print_callback(tempStr.c_str());

		free(temp);
	}
#endif

#endif
}