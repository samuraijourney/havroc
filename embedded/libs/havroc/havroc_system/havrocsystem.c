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

static uint8_t m_board_arm;

void HeartBeat(event EventReceived)
{
	sendMessage message;

	if(EventReceived.command == SYSTEM_HEART_BEAT)
	{
		message.module = SYSTEM_MOD;
		message.command = SYSTEM_HEART_BEAT;
		message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x00) & 0x00FF);
		message.arm = m_board_arm;

		WiFiSendEnQ(message);
	}
}

void SystemRun(UArg arg0, UArg arg1)
{
	long result = WiFiStartup(m_board_arm);
	uint8_t i = 0;
	uint8_t imu_select[2];
	uint8_t retVal;
	uint8_t imu_setup = 0;

	imu_select[0] = (m_board_arm ==  RIGHT_ARM ? R_SHOULDER_IMU_ID : L_SHOULDER_IMU_ID);
	imu_select[1] = (m_board_arm ==  RIGHT_ARM ? R_ELBOW_IMU_ID : L_ELBOW_IMU_ID);

	if(result != 0)
	{
		Task_exit();
	}

	retVal = Setup_IMUs(imu_select, 2, m_board_arm);

	if(retVal != SUCCESS)
	{
		Tracking_Publish_Error(retVal);
	}
	else
	{
		imu_setup = 1;
	}

	EventRegisterCB(SYSTEM_MOD, HeartBeat);

	while (1)
	{

		if(imu_setup)
		{
			Tracking_Update(2);

			if(i == SEND_DELAY)
			{
				Tracking_Publish();
				i = 0;
			}
			else
			{
				i++;
			}
		}

		else
		{
			retVal = Setup_IMUs(imu_select, 2, m_board_arm);

			if(retVal != SUCCESS)
			{
				Tracking_Publish_Error(retVal);
			}
			else
			{
				imu_setup = 1;
			}
		}

		WiFiSend();

		WiFiReceive();

		Task_yield();
	}
}

void SystemStartTask(uint8_t arm)
{
	m_board_arm = arm;
	Task_Handle task1 = Task_Object_get(NULL, 1);
	Task_setPri(task1, 10);
}
