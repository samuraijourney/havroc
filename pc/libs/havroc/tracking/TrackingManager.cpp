#include <havroc/tracking/TrackingManager.h>
#include <havroc/common/CommandManager.h>

namespace havroc
{

	TrackingManager* TrackingManager::m_instance = 0;

	TrackingManager* TrackingManager::get()
	{
		if (m_instance == 0)
		{
			m_instance = new TrackingManager();
		}

		return m_instance;
	}

	TrackingManager::TrackingManager()
	: m_active(false)
	{
		CommandManager::get()->register_tracking_callback<TrackingManager>(&TrackingManager::tracking_command_handler, this);
	}

	void TrackingManager::tracking_command_handler(havroc::command_pkg* pkg)
	{
		switch (pkg->command)
		{
			case TRACKING_STATE_CMD:
			{
				if (!m_active)
				{
					m_active = pkg->data[0];
					m_active ? m_start_event() : m_end_event();
				}

				break;
			}
			case TRACKING_DATA_CMD:
			{
				float angles[ANGLES_PER_ARM];

				for (int i = 0; i < ANGLES_PER_ARM; i++)
				{
					memcpy(&angles[i], &pkg->data[sizeof(float)*i+1], sizeof(float));
				}

				m_shoulder_event(angles[0], angles[1], angles[2], pkg->data[0]);
				m_elbow_event(angles[3], angles[4], angles[5], pkg->data[0]);

				break;
			}
		}
	}

}