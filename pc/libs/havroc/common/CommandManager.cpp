#include <havroc\communications\NetworkManager.h>
#include <havroc\common\CommandManager.h>

namespace havroc
{

	CommandManager* CommandManager::m_instance = 0;

	CommandManager* CommandManager::get()
	{
		if (m_instance == 0)
		{
			m_instance = new CommandManager();
		}

		return m_instance;
	}

	CommandManager::CommandManager()
	{
		uint8_t connections = TCP_CLIENT | TCP_SERVER | UDP_CLIENT;

		NetworkManager::get()->register_receive_callback<CommandManager>(&CommandManager::receive_handler, this, connections);
	
		boost::thread(boost::bind(&CommandManager::event_loop, this));
	}

	CommandManager::~CommandManager(){}

	void CommandManager::event_loop()
	{
		while (true)
		{
			if (!m_pkgs.empty())
			{
				command_pkg* pkg = m_pkgs.front();

				switch (pkg->command)
				{
					case(TRACKING_CMD) :
					{
						m_tracking_event(pkg);
						break;
					}
					case(SYSTEM_CMD) :
					{
						m_system_event(pkg);
						break;
					}
					case(MOTOR_CMD) :
					{
						m_motor_event(pkg);
						break;
					}
					default:
					{
						printf("Invalid event received, ignoring command event\n");
						break;
					}
				}

				m_pkg_buffer_lock.lock();
				m_pkgs.erase(m_pkgs.begin());
				m_pkg_buffer_lock.unlock();

				delete pkg;
			}

			if (NetworkManager::get()->is_active())
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(50));
			}
			else
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
			}
		}
	}

	void CommandManager::receive_handler(char* msg, size_t size)
	{
		if (!CommandBuilder::is_command(msg, size))
		{
			return;
		}

		command_pkg* pkg = new command_pkg();
		CommandBuilder::parse_command(pkg, msg, size);

		m_pkg_buffer_lock.lock();
		m_pkgs.push_back(pkg);
		m_pkg_buffer_lock.unlock();
	}

}