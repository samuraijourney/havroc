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
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

/* Driverlib Includes */
#include "uart_if.h"
#include "common.h"

/* HaVRoc library includes */
#include "havroc/command.h"
#include "havroc/eventmgr/eventmgr.h"
#include <havroc/error.h>

#define EventReceived 1

static EVENT_CB 	  EventList[CMD_MAX][MAX_CALLBACKS];
static int			  callbackCounter[CMD_MAX];
extern Event_Handle   EventMgr_Event;

event				  eventBuff[EVENT_BUFF_SIZE];
int				      eventFront;
int		 		      eventBack;
int 				  eventCount;

int EventRegisterCB(int32_t command, EVENT_CB Callback)
{
	int i;

	if(Callback && (command >= CMD_BASE) && (command < CMD_MAX))
	{
		EVENT_CB *pEntry = &EventList[command][0];

		for (i = 0; i < MAX_CALLBACKS; i++, pEntry++)
		{
			if (*pEntry == 0)
			{
				*pEntry = Callback;
				callbackCounter[command]++;
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

	Report("In Event Handler: Received %i command \n\r", currEvent.command);

	if((currEvent.command >= CMD_BASE) && (currEvent.command < CMD_MAX) && (callbackCounter[currEvent.command] > 0))
	{
		EVENT_CB *pEntry = &EventList[currEvent.command][0];

		for (i = 0; i < callbackCounter[currEvent.command]; i++, pEntry++)
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

		eventBuff[eventFront].command = message[buff_index++];
		eventBuff[eventFront].data_len = message[buff_index++];
		eventBuff[eventFront].data_len <<= 8;
		eventBuff[eventFront].data_len |= message[buff_index++];
		eventBuff[eventFront].data_buff = (uint8_t*) malloc(sizeof(uint8_t)*eventBuff[eventFront].data_len);

		while(data_index < eventBuff[eventFront].data_len)
		{
			eventBuff[eventFront].data_buff[data_index++] = message[buff_index++];
		}

		Report("TCP received command: %i, size: %i\n\r", eventBuff[eventFront].command, eventBuff[eventFront].data_len);

		eventCount++;
		Event_post(EventMgr_Event, EventReceived);
    }

	return buff_index;
}

int EventStart()
{
	Task_Handle task0;
	Task_Params params;

	Task_Params_init(&params);
	params.instance->name = "EventRun_Task";
	params.priority = 10;

	task0 = Task_create((Task_FuncPtr) EventRun, &params, NULL);
	if (task0 == NULL || EventMgr_Event == NULL)
	{
		signal(EVENT_START_FAIL);
		return EVENT_START_FAIL;
	}

	return SUCCESS;
}

void EventRun (UArg arg0, UArg arg1)
{
	UInt events;
	float prev = 0;
	float now = 0;
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	InitTerm();

	while(1)
	{
		prev = Timestamp_get32()/(1.0*freq.lo);

		System_printf("In Event Handler, Waiting for Event \n");
		System_flush();

		events = Event_pend(EventMgr_Event, EventReceived, Event_Id_NONE, BIOS_WAIT_FOREVER);

		System_printf("In Event Handler, Event received \n");
		System_flush();

		if(events & EventReceived)
		{
			EventFire(eventBuff[eventBack]);
		}
		now = Timestamp_get32()/(1.0*freq.lo);

		System_printf("This is Task 2 - Elapsed Time is %.04f\n", now-prev);
		System_flush();
	}
}
