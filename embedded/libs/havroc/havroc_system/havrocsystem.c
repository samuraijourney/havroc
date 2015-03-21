/*
 * system.c
 *
 *  Created on: Mar 20, 2015
 *      Author: Akram
 */


/* BIOS Header files */
#include <ti/sysbios/knl/Task.h>

/* HaVRoc Library Includes */
#include "havroc/communications/radio/wifi_communication.h"
#include <havroc/error.h>
#include <havroc/id.h>
#include <havroc/command.h>
#include <havroc/tracking/Tracking_Manager.h>
#include <havroc/eventmgr/eventmgr.h>
#include <havroc/havroc_system/havrocsystem.h>
#include <havroc/havroc_utils/havrocutils.h>

#define SEND_DELAY 5

void HeartBeat(event EventReceived)
{
	sendMessage message;

	if(EventReceived.command == SYSTEM_HEART_BEAT)
	{
		message.module = SYSTEM_MOD;
		message.command = SYSTEM_HEART_BEAT;
		message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x00) & 0x00FF);
		message.arm = RIGHT_ARM;

		WiFiSendEnQ(message);
	}
}

void SystemRun(UArg arg0, UArg arg1)
{
	long result = WiFiStartup();
	int i = 0;

	if(result != 0)
	{
		Task_exit();
	}

	Setup_IMUs(R_SHOULDER_IMU_ID, 1);

	EventRegisterCB(SYSTEM_MOD, HeartBeat);

	while (1)
	{
		Tracking_Update(R_SHOULDER_IMU_ID, 1);

		if(i == SEND_DELAY)
		{
			Tracking_Publish();

			WiFiSend();

			i = 0;
		}
		else
		{
			i++;
		}

		WiFiReceive();

		Task_yield();
	}
}

void SystemStartTask()
{
	Task_Handle task1 = Task_Object_get(NULL, 1);
	Task_setPri(task1, 10);
}
