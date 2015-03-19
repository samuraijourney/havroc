#ifndef LOGGER_H_
#define LOGGER_H_

#define PRINT_REMOTE
#define PRINT_LOCAL

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sstream>

#define LOG_STRING_LIMIT_IN_BYTES	4096

namespace havroc
{
	typedef void(*RemotePrintCallback)(const char *);

	class Logger
	{
		public:
			Logger();
			~Logger();

			static void set_remote_print_func(RemotePrintCallback func) { m_remotePrintCallback = func; }

			static std::stringstream log;

			static void start_logger();
			static void stop_logger();

		private:
			static void logger_loop();

			static bool m_active;

			static RemotePrintCallback m_remotePrintCallback;
	};
} /* namespace havroc */

#endif /* LOGGER_H_ */

