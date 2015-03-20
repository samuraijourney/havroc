#ifndef LOGGER_H_
#define LOGGER_H_

//#define NO_LOGGER

#define PRINT_LOCAL_AND_REMOTE
//#define PRINT_REMOTE
//#define PRINT_LOCAL

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>

#define LOG_STRING_LIMIT_IN_BYTES	4096

#define LOG_INFO		0
#define LOG_WARNING		1
#define LOG_ERROR		2

#define FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define LOG(a,b,...) Logger::log(FILE_NAME, __LINE__, (a), (b), __VA_ARGS__)

namespace havroc
{
	typedef void(*RemotePrintCallback)(unsigned char type, const char *);

	class Logger
	{
		public:
			Logger();
			~Logger();

			static void set_remote_print_func(RemotePrintCallback func) { m_remote_print_callback = func; }

#ifndef NO_LOGGER
			static void log(const char* file_name, int line_number, unsigned char type, const char *format, ...);
#else
			static void log(...){}
#endif

		private:
			static RemotePrintCallback m_remote_print_callback;
			static std::string   	   m_type_text[3];
	};
} /* namespace havroc */

#endif /* LOGGER_H_ */

