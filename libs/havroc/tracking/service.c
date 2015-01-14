#include <stdbool.h>

#ifdef cc3200
	#include <xdc/runtime/System.h>
	#include <xdc/runtime/Error.h>
	#include <ti/sysbios/BIOS.h>
	#include <ti/sysbios/knl/Event.h>
	#include <ti/sysbios/knl/Mailbox.h>
#elif TIVAWARE
	#include <xdc/runtime/System.h>
	#include <xdc/runtime/Error.h>
	#include <ti/sysbios/BIOS.h>
	#include <ti/sysbios/knl/Event.h>
	#include <ti/sysbios/knl/Mailbox.h>
#endif

#include <havroc/tracking/service.h>
#include <havroc/tracking/estimation.h>
#include <havroc/communications/radio/radio_comm.h>

#define MAX_ATTEMPTS 100

static bool _active = false;

static float _s_yaw;
static float _s_pitch;
static float _s_roll;
static float _e_yaw;
static float _e_pitch;
static float _e_roll;
static float _w_yaw;
static float _w_pitch;
static float _w_roll;

static Event_Handle _event;
static Mailbox_Handle _mbox;

// Temporary function for testing service end
void kill_test(UArg arg0, UArg arg1)
{
	if(!end_service())
	{
		System_printf("Service end success\n");
	}
	else
	{
		System_printf("Service end error\n");
	}
	System_flush();
}


int start_service()
{
	_active = true;

	Error_Block eb;
	Error_init(&eb);

	Task_Params task_params;
	Task_Handle task;

	Task_Params_init(&task_params);
	task_params.stackSize = 512;
	task_params.priority = 15;

	task = Task_create(execute, &task_params, &eb);
	if (task == NULL)
	{
		return SERVICE_START_FAIL;
	}

	_event = Event_create(NULL, &eb);
	if (_event == NULL)
	{
		return SERVICE_START_FAIL;
	}

	Mailbox_Params mbox_params;
	Mailbox_Params_init(&mbox_params);
	mbox_params.readerEvent = _event;
	mbox_params.readerEventId = Event_Id_00;

	_mbox = Mailbox_create(sizeof(int), 5, &mbox_params, &eb);
	if (_mbox == NULL)
	{
		System_abort("Mailbox create failed");
	}

    System_printf("Mailbox created\n");
	System_flush();

	return 0;
}

int end_service()
{
	int result = -1;
	_active = false;

    System_printf("Ending service\n");
	System_flush();

	while (true)
	{
		UInt exit_event = Event_pend( _event,
							 	      Event_Id_NONE, 		/* andMask = 0 */
									  Event_Id_00, 			/* orMask */
									  BIOS_WAIT_FOREVER ); 	/* timeout */
		if (exit_event & Event_Id_00)
		{
			Mailbox_pend(_mbox, &result, BIOS_NO_WAIT);
			break;
		}
	}

    System_printf("Service end error code received\n");
	System_flush();

	Mailbox_delete(&_mbox);

	return result;
}

int terminate_service()
{
	_active = false;
	signal(SERVICE_KILLED);
	return SERVICE_KILLED;
}

void execute(UArg arg0, UArg arg1)
{
	int result = 0;
	int cnt = 0; // Task kill hack variable

	while(_active)
	{
		int attempts = 0;

		while(attempts < MAX_ATTEMPTS)
		{
			if( !(!get_shoulder_imu(&_s_yaw,&_s_pitch,&_s_roll) &&
				  !get_elbow_imu(&_e_yaw,&_e_pitch,&_e_roll)    &&
				  !get_wrist_imu(&_w_yaw,&_w_pitch,&_w_roll)) )
			{
				attempts++;
			}
			else
			{
				break;
			}
		}

		if(attempts == MAX_ATTEMPTS)
		{
			result = terminate_service();
			break;
		}

		result = publish( _s_yaw,_s_pitch,_s_roll,
						  _e_yaw,_e_pitch,_e_roll,
						  _w_yaw,_w_pitch,_w_roll );

		if(result != 0)
		{
			break;
		}

    	System_printf("Publishing frame\n");
    	System_flush();

// Task kill hack test start
    	if(cnt >= 20)
    	{
    		Error_Block eb;
			Error_init(&eb);

			Task_Params task_params;
			Task_Handle task;

			Task_Params_init(&task_params);
			task_params.stackSize = 512;
			task_params.priority = 15;

			task = Task_create(kill_test, &task_params, &eb);
			if (task == NULL)
			{
				return;
			}
    	}
    	else
    	{
    		cnt++;
    	}
// Task kill hack test end

    	Task_yield();
	}

    System_printf("Posting service stop error code\n");
	System_flush();
	Mailbox_post(_mbox, &result, BIOS_WAIT_FOREVER);
}

int publish(  float s_yaw,
	          float s_pitch,
	          float s_roll,
	          float e_yaw,
	          float e_pitch,
	          float e_roll,
	          float w_yaw,
	          float w_pitch,
	          float w_roll )
{
	return 0; //send_frame( s_yaw,s_pitch,s_roll,
		      //         e_yaw,e_pitch,e_roll,
		      //         w_yaw,w_pitch,w_roll );
}

int get_imu( int id,
			 float* yaw,
	         float* pitch,
	         float* roll )
{
	return 0;
}

int get_shoulder_imu( float* s_yaw,
			          float* s_pitch,
			          float* s_roll )
{
	return get_imu(SHOULDER_IMU_ID,s_yaw,s_pitch,s_roll);
}

int get_elbow_imu( float* e_yaw,
		           float* e_pitch,
			       float* e_roll )
{
	return get_imu(ELBOW_IMU_ID,e_yaw,e_pitch,e_roll);
}

int get_wrist_imu( float* w_yaw,
		           float* w_pitch,
			       float* w_roll )
{
	return get_imu(WRIST_IMU_ID,w_yaw,w_pitch,w_roll);
}

void signal( int error )
{
	return; //send_error(error);
}

