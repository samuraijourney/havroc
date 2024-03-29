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
				float angles[2 * ANGLES_PER_ARM];

				for (int i = 0; i < 2 * ANGLES_PER_ARM; i++)
				{
					memcpy(&angles[i], &pkg->data[sizeof(float)*i], sizeof(float));
				}

				m_shoulder_event(angles[0], angles[1], angles[2], RIGHT_ARM);
				m_elbow_event(angles[3], angles[4], angles[5], RIGHT_ARM);
				m_wrist_event(angles[6], angles[7], angles[8], RIGHT_ARM);

				m_shoulder_event(angles[9], angles[10], angles[11], LEFT_ARM);
				m_elbow_event(angles[12], angles[13], angles[14], LEFT_ARM);
				m_wrist_event(angles[15], angles[16], angles[17], LEFT_ARM);

				break;
			}
		}
	}

}