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
	: m_clear_buffer(false)
	{
		uint8_t connections = TCP_CLIENT | TCP_SERVER | UDP_CLIENT;

		NetworkManager::get()->register_reset_callback<CommandManager>(&CommandManager::network_reset, this, connections);
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

				switch (pkg->module)
				{
					case(TRACKING_MOD) :
					{
						m_tracking_event(pkg);
						break;
					}
					case(SYSTEM_MOD) :
					{
						m_system_event(pkg);
						break;
					}
					case(MOTOR_MOD) :
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

				if (m_clear_buffer)
				{
					m_pkg_buffer_lock.lock();
					m_pkgs.clear();
					m_pkg_buffer_lock.unlock();

					m_clear_buffer = false;
				}
			}

			if (NetworkManager::get()->is_active())
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}
			else
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
			}
		}
	}

	void CommandManager::receive_handler(BYTE* msg, size_t size)
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

	void CommandManager::network_reset(bool receive_attached)
	{
		if (!receive_attached)
		{
			uint8_t connections = TCP_CLIENT | TCP_SERVER | UDP_CLIENT;

			NetworkManager::get()->register_receive_callback<CommandManager>(&CommandManager::receive_handler, this, connections);

			m_clear_buffer = true;
		}
	}
}