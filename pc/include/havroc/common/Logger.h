#ifndef LOGGER_H_
#define LOGGER_H_

//#define NO_LOGGER

#define PRINT_LOCAL_AND_REMOTE
//#define PRINT_REMOTE
//#define PRINT_LOCAL

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG_STRING_LIMIT_IN_BYTES	4096

#define FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define LOG(a,...) Logger::log(FILE_NAME, __LINE__, (a), __VA_ARGS__)

namespace havroc
{
	typedef void(*RemotePrintCallback)(const char *);

	class Logger
	{
		public:
			Logger();
			~Logger();

			static void set_remote_print_func(RemotePrintCallback func) { m_remote_print_callback = func; }

#ifndef NO_LOGGER
			static void log(const char* file_name, int line_number, const char *format, ...);
#else
			static void log(const char *format, ...){}
#endif

		private:
			static RemotePrintCallback m_remote_print_callback;
	};
} /* namespace havroc */

#endif /* LOGGER_H_ */

