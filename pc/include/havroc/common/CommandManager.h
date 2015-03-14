#ifndef COMMANDMANAGER_H_
#define COMMANDMANAGER_H_

#include <mutex>

#include <havroc\communications\NetworkManager.h>
#include <havroc\common\CommandBuilder.h>

namespace havroc
{

	class CommandManager
	{
	public:
		~CommandManager();

		static CommandManager* get();

		template<class T>
		void register_tracking_callback(void(T::*tracking_callback)(command_pkg* pkg), T* obj)
		{
			m_tracking_event.connect(boost::bind(tracking_callback, obj, _1));
		}

		void register_tracking_callback(void(*tracking_callback)(command_pkg* pkg))
		{
			m_tracking_event.connect(tracking_callback);
		}

		template<class T>
		void register_system_callback(void(T::*system_callback)(command_pkg* pkg), T* obj)
		{
			m_system_event.connect(boost::bind(system_callback, obj, _1));
		}

		void register_system_callback(void(*system_callback)(command_pkg* pkg))
		{
			m_system_event.connect(system_callback);
		}

		template<class T>
		void register_motor_callback(void(T::*motor_callback)(command_pkg* pkg), T* obj)
		{
			m_motor_event.connect(boost::bind(motor_callback, obj, _1));
		}

		void register_motor_callback(void(*motor_callback)(command_pkg* pkg))
		{
			m_motor_event.connect(motor_callback);
		}

		template<class T>
		void register_error_callback(void(T::*error_callback)(command_pkg* pkg), T* obj)
		{
			m_error_event.connect(boost::bind(error_callback, obj, _1));
		}

		void register_error_callback(void(*error_callback)(command_pkg* pkg))
		{
			m_error_event.connect(error_callback);
		}

		template<class T>
		void unregister_tracking_callback(void(T::*tracking_callback)(command_pkg* pkg), T* obj)
		{
			m_tracking_event.disconnect(boost::bind(tracking_callback, obj, _1));
		}

		void unregister_tracking_callback(void(*tracking_callback)(command_pkg* pkg))
		{
			m_tracking_event.disconnect(tracking_callback);
		}

		template<class T>
		void unregister_system_callback(void(T::*system_callback)(command_pkg* pkg), T* obj)
		{
			m_system_event.disconnect(boost::bind(system_callback, obj, _1));
		}

		void unregister_system_callback(void(*system_callback)(command_pkg* pkg))
		{
			m_system_event.disconnect(system_callback);
		}

		template<class T>
		void unregister_motor_callback(void(T::*motor_callback)(command_pkg* pkg), T* obj)
		{
			m_motor_event.disconnect(boost::bind(motor_callback, obj, _1));
		}

		void unregister_motor_callback(void(*motor_callback)(command_pkg* pkg))
		{
			m_motor_event.disconnect(motor_callback);
		}

		template<class T>
		void unregister_error_callback(void(T::*error_callback)(command_pkg* pkg), T* obj)
		{
			m_error_event.disconnect(boost::bind(error_callback, obj, _1));
		}

		void unregister_error_callback(void(*error_callback)(command_pkg* pkg))
		{
			m_error_event.disconnect(error_callback);
		}

	private:
		CommandManager();

		void network_reset(bool receive_attached);
		void receive_handler(BYTE* msg, size_t size);
		void event_loop();

		boost::signals2::signal<void(command_pkg*)> m_tracking_event;
		boost::signals2::signal<void(command_pkg*)> m_system_event;
		boost::signals2::signal<void(command_pkg*)> m_motor_event;
		boost::signals2::signal<void(command_pkg*)> m_error_event;

		std::vector<command_pkg*> m_pkgs;
		std::mutex m_pkg_buffer_lock;

		bool m_clear_buffer;

		static CommandManager* m_instance;

	};

} /* namespace havroc */

#endif /* COMMANDMANAGER_H_ */

