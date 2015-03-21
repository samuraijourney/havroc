#include <string>
#include <mutex>

#include <boost/thread/thread.hpp>
#include <havroc/common/Logger.h>

std::mutex _console_lock;

namespace havroc
{
	RemotePrintCallback Logger::m_remote_print_callback = 0;
	std::string Logger::m_type_text[3] = { "INFO", "WARNING", "ERROR" };

#ifndef NO_LOGGER

#if defined(PRINT_LOCAL)
	void Logger::log(const char* file_name, int line_number, unsigned char type, const char *format, ...)
	{
		va_list arg;

		std::string typeStr = m_type_text[type];

		_console_lock.lock();
		printf("%s:%s:%d ", typeStr.c_str(), file_name, line_number);
		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);
		_console_lock.unlock();
	}

#elif defined(PRINT_REMOTE)
	void Logger::log(const char* file_name, int line_number, unsigned char type, const char *format, ...)
	{
		if (!m_remote_print_callback)
		{
			return;
		}

		va_list arg;

		std::string typeStr = m_type_text[type];

		char* temp = (char*)malloc(LOG_STRING_LIMIT_IN_BYTES);
		sprintf(temp, "%s:%s:%d ", typeStr.c_str(), file_name, line_number);
		int length = strlen(std::string(temp).c_str());

		va_start(arg, format);
		vsprintf(&temp[length], format, arg);
		va_end(arg);

		std::string tempStr = temp;

		m_remote_print_callback(tempStr.c_str());

		free(temp);
	}

#elif defined(PRINT_LOCAL_AND_REMOTE)
	void Logger::log(const char* file_name, int line_number, unsigned char type, const char *format, ...)
	{
		va_list arg;

		std::string typeStr = m_type_text[type];

		_console_lock.lock();
		printf("%s:%s:%d ", typeStr.c_str(), file_name, line_number);
		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);
		_console_lock.unlock();

		if (!m_remote_print_callback)
		{
			return;
		}

		char* temp = (char*)malloc(LOG_STRING_LIMIT_IN_BYTES);
		sprintf(temp, "%s:%s:%d ", typeStr.c_str(), file_name, line_number);
		int length = strlen(std::string(temp).c_str());

		va_start(arg, format);
		vsprintf(&temp[length], format, arg);
		va_end(arg);

		std::string tempStr = temp;

		m_remote_print_callback(type, tempStr.c_str());

		free(temp);
	}
#endif

#endif
}