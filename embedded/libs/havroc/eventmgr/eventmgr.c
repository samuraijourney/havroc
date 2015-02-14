/*
 * eventmgr.c
 *
 *  Created on: Feb 3, 2015
 *      Author: Akram
 */


#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include "havroc/command.h"
#include "havroc/eventmgr/eventmgr.h"
#include <stdlib.h>
#include "uart_if.h"
#include "common.h"
#include <havroc/error.h>

#define EventReceived 0

static EVENT_CB 	  EventList[CMD_MAX][MAX_CALLBACKS];
static int			  callbackCounter[CMD_MAX];
Event_Handle 		  EventMgr_Event;

event				  eventBuff[EVENT_BUFF_SIZE];
int				      eventFront;
int		 		      eventBack;
int 				  eventCount;

int EventRegisterCB(uint32_t command, EVENT_CB Callback)
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

	EventMgr_Event = Event_create(NULL, NULL);
	task0 = Task_create((Task_FuncPtr) EventRun, NULL, NULL);
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

	while(1)
	{
		events = Event_pend(EventMgr_Event, EventReceived, Event_Id_NONE, BIOS_WAIT_FOREVER);

		if(events & EventReceived)
		{
			EventFire(eventBuff[eventBack]);
		}
	}
}
