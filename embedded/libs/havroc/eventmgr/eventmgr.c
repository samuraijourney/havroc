/*
 * eventmgr.c
 *
 *  Created on: Feb 3, 2015
 *      Author: Akram
 */

/* Standard C Includes */
#include <stdlib.h>

/* SYS/BIOS Includes */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/System.h>

/* HaVRoc library includes */
#include "havroc/command.h"
#include "havroc/eventmgr/eventmgr.h"
#include "havroc/communications/radio/wifi_communication.h"
#include <havroc/error.h>
#include <havroc/havroc_utils/havrocutils.h>

#define EventReceived 1

static EVENT_CB 	  EventList[MAX_MOD][MAX_CALLBACKS];
static int			  callbackCounter[MAX_MOD];
event				  eventBuff[EVENT_BUFF_SIZE];
int				      eventFront;
int		 		      eventBack;
int 				  eventCount;
Event_Handle		  Command_Received_Handle;

int EventRegisterCB(int32_t module, EVENT_CB Callback)
{
	int i;

	if(Callback && (module >= BASE_MOD) && (module < MAX_MOD))
	{
		EVENT_CB *pEntry = &EventList[module][0];

		for (i = 0; i < MAX_CALLBACKS; i++, pEntry++)
		{
			if (*pEntry == 0)
			{
				*pEntry = Callback;
				callbackCounter[module]++;
			}
		}
	}
	else
	{
		return FAILURE;
	}

	return SUCCESS;
}

static int EventFire(event currEvent)
{
	int i;

	//Report("In Event Handler: Received %i command \n\r", currEvent.command);
	if((currEvent.module >= BASE_MOD) && (currEvent.module < MAX_MOD) && (callbackCounter[currEvent.module] > 0))
	{
		EVENT_CB *pEntry = &EventList[currEvent.module][0];

		for (i = 0; i < callbackCounter[currEvent.module]; i++, pEntry++)
		{
            (*pEntry)(currEvent);
		}

		eventCount--;
		eventBack = (eventBack + 1)%(EVENT_BUFF_SIZE);
		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}
}

int EventEnQ(char* message)
{
    int	data_index = 0;
    int	buff_index = 0;
    int event_index = (eventFront + 1)%(EVENT_BUFF_SIZE);

    if((event_index != eventBack) || (eventCount == 0))
    {
    	eventFront = event_index;

    	eventBuff[eventFront].module = message[buff_index++];
		eventBuff[eventFront].command = message[buff_index++];
		eventBuff[eventFront].data_len = message[buff_index++];
		eventBuff[eventFront].data_len <<= 8;
		eventBuff[eventFront].data_len |= message[buff_index++];
		eventBuff[eventFront].data_buff = (uint8_t*) malloc(sizeof(uint8_t)*eventBuff[eventFront].data_len);

		while(data_index < eventBuff[eventFront].data_len)
		{
			eventBuff[eventFront].data_buff[data_index++] = message[buff_index++];
		}

		//Report("TCP received module %i, command: %i, size: %i\n\r", eventBuff[eventFront].module, eventBuff[eventFront].command, eventBuff[eventFront].data_len);

		eventCount++;
		Event_post(Command_Received_Handle, EventReceived);
    }

	return buff_index;
}

void EventStart()
{
	Task_Handle task0 = Task_Object_get(NULL, 0);
	Task_setPri(task0, 10);

	Command_Received_Handle = Event_Object_get(NULL, 0);
}

void EventRun (UArg arg0, UArg arg1)
{
	while(1)
	{
		while(!(isWiFiActive()))
		{
			Task_sleep(5);
		}

		Event_pend(Command_Received_Handle, EventReceived, Event_Id_NONE, BIOS_WAIT_FOREVER);

		if(eventBack != eventFront)
		{
			EventFire(eventBuff[eventBack]);
		}

		Task_yield();
	}
}
