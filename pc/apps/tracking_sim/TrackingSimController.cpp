#include <mutex>

#include <boost/thread/thread.hpp>

#include <havroc/communications/NetworkManager.h>
#include <havroc/common/CommandBuilder.h>

#include "TrackingSimController.h"


TrackingSimController::TrackingSimController(std::string file_path, BYTE arm)
: m_file_path(file_path),
  m_loaded(false),
  m_stop(false),
  m_playing(false),
  m_pause(false),
  m_arm(arm){}

TrackingSimController::~TrackingSimController(){}

void TrackingSimController::change(bool repeat, int action_id)
{
	m_stop = true;

	while (m_playing);

	m_stop = false;

	play(repeat, action_id);
}

void TrackingSimController::pause()
{
	m_pause = true;
}

void TrackingSimController::resume()
{
	m_pause = false;
}

void TrackingSimController::stop()
{
	m_stop = false;
}

void TrackingSimController::play(bool repeat, int action_id)
{
	boost::thread(boost::bind(&TrackingSimController::execute, this, repeat, action_id));
}

void TrackingSimController::execute(bool repeat, int action_id)
{
	m_play_lock.lock();

	bool success;

	if (!m_loaded)
	{
		success = load_data();

		if (!success)
		{
			m_play_lock.unlock();

			return;
		}
	}

	m_playing = true;

	if (action_id == -1)
	{
		do
		{
			for (int i = 0; i < ACTIONS_COUNT; i++)
			{
				success = play_action(i, false);

				if (!success)
				{
					m_play_lock.unlock();

					return;
				}
			}

			if (m_stop)
			{
				break;
			}

		} while (repeat);
	}
	else
	{
		success = play_action(action_id, repeat);
		
		if (!success)
		{
			m_play_lock.unlock();

			return;
		}
	}

	m_playing = false;

	m_play_lock.unlock();
}

bool TrackingSimController::play_action(int action_id, bool repeat)
{
	if (action_id >= ACTIONS_COUNT || action_id < 0)
	{
		return false;
	}

	action selected_action = m_actions[action_id];

	do
	{
		for (int i = 0; i < READINGS_PER_ACTION; i++)
		{
			BYTE* msg;
			size_t size;

			while (m_pause)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(100));
			}

			if (m_arm == LEFT_ARM)
			{
				havroc::CommandBuilder::build_tracking_data_sim_command(msg, size, selected_action.data[i].left_angles, m_arm);
				if (int failures = havroc::NetworkManager::get()->send(msg, size, true))
				{
					return false;
				}
			}
			else if (m_arm == RIGHT_ARM)
			{
				havroc::CommandBuilder::build_tracking_data_sim_command(msg, size, selected_action.data[i].right_angles, m_arm);
				if (int failures = havroc::NetworkManager::get()->send(msg, size, true))
				{
					return false;
				}
			}

			if (m_stop)
			{
				break;
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}

		if (m_stop)
		{
			break;
		}

	} while (repeat);

	return true;
}

bool TrackingSimController::load_data()
{
	m_file = fopen(m_file_path.c_str(), "r");

	if (!m_file)
	{
		printf("Unable to load data\n");
		return false;
	}

	int size = 1024;
	char* buffer = (char*)malloc(sizeof(char) * size);

	read_line(buffer, size); // Read header data

	for (int i = 0; i < ACTIONS_COUNT; i++)
	{
		m_actions[i].action_id = i;

		for (int j = 0; j < READINGS_PER_ACTION; j++)
		{
			char* data[DATA_PER_LINE];

			read_line(buffer, size);
			parse_line(buffer, data);
			populate_tracking_packet(m_actions[i].data[j], data);
		}
	}

	free(buffer);

	fclose(m_file);

	m_loaded = true;

	return true;
}

void TrackingSimController::read_line(char* buffer, int size)
{
	fgets(buffer, size, m_file);
}

void TrackingSimController::parse_line(char* buffer, char* tokens[DATA_PER_LINE])
{
	tokens[0] = strtok(buffer, ",");
	for(int i = 1; i < DATA_PER_LINE; i++)
	{
		tokens[i] = strtok(NULL, ",");
	}
}

void TrackingSimController::populate_tracking_packet(tracking_packet& pkg, char* data[DATA_PER_LINE])
{
	for (int i = 0; i < ANGLES_PER_ARM; i++)
	{
		pkg.right_angles[i] = (float)atof(data[i+1]);
	}

	for (int i = ANGLES_PER_ARM + ANGLES_PER_IMU; i < 2 * ANGLES_PER_ARM + ANGLES_PER_IMU; i++)
	{
		pkg.left_angles[i - ANGLES_PER_ARM - ANGLES_PER_IMU] = (float)atof(data[i + 1]);
	}
}
