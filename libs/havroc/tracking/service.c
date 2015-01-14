#include <havroc/tracking/service.h>

#ifdef cc3200
#include <xdc/runtime/System.h>
#endif

#define MAX_ATTEMPTS 100

static bool _active = false;

float _s_yaw;
float _s_pitch;
float _s_roll;
float _e_yaw;
float _e_pitch;
float _e_roll;
float _w_yaw;
float _w_pitch;
float _w_roll;

int start_service()
{
	_active = true;
	//signal(SERVICE_START_SUCCESS);

	System_printf("Starting service\n");
	System_flush();

	return 0;
}
