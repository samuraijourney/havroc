#ifndef TRACKINGSIMCONTROLLER_H_
#define TRACKINGSIMCONTROLLER_H_

#include <string>

#include <havroc/tracking/TrackingDefinitions.h>

#define ACTIONS_COUNT	6
#define READINGS_PER_ACTION 1000
#define DATA_PER_LINE 19

#define ACTION_STATIC_ANTERIOR_PARALLEL			0
#define ACTION_STATIC_ANTERIOR_PARALLEL_90		1
#define ACTION_STATIC_NEUTRAL_BOXING			2
#define ACTION_STATIC_NEUTRAL_SIDE				3
#define ACTION_STATIC_LATERAL_PARALLEL			4
#define ACTION_DYNAMIC_BOXING					5

class TrackingSimController
{
	typedef struct _tracking_packet
	{
		float angles[2 * ANGLES_PER_ARM];
	} tracking_packet;

	typedef struct _action
	{
		int action_id;
		tracking_packet data[READINGS_PER_ACTION];
	} action;

public:
	TrackingSimController(std::string file_path);
	virtual ~TrackingSimController();

	void play(bool repeat, int action_id = -1);
	void pause();
	void resume();
	void stop();
	void change(bool repeat, int action_id);

private:
	void execute(bool repeat, int action_id = -1);
	bool play_action(int action_id, bool repeat);
	
	bool load_data();

	void read_line(char* buffer, int size);
	void parse_line(char* buffer, char* tokens[DATA_PER_LINE]);
	void populate_tracking_packet(tracking_packet& pkg, char* data[DATA_PER_LINE]);
	
	action m_actions[ACTIONS_COUNT];
	std::string m_file_path;
	std::mutex m_play_lock;
	FILE* m_file;

	bool m_loaded;
	bool m_playing;
	bool m_pause;
	bool m_stop;
};

#endif /* TRACKINGSIMCONTROLLER_H_ */
