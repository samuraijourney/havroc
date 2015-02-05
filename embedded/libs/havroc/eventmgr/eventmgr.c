/*
 * eventmgr.c
 *
 *  Created on: Feb 3, 2015
 *      Author: Akram
 */

#include "havroc/command.h"
#include "havroc/eventmgr/eventmgr.h"
#include <stdlib.h>
#include "uart_if.h"

static EVENT_CB 	  EventList[CMD_MAX][MAX_CALLBACKS];
extern event		  eventBuff[EVENT_BUFF_SIZE];
extern int		      eventIndex;
extern int 			  eventCount;
static int			  callbackCounter[CMD_MAX];

void EventRegisterCB(uint32_t command, EVENT_CB Callback)
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
				break;
			}
		}
	}
	else
	{
		//invalid callback or command
	}
}

static void EventFire(event currEvent)
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
		eventIndex--;

		if(eventIndex < 0)
		{
			eventIndex+=EVENT_BUFF_SIZE;
		}
	}
	else
	{
		//invalid command
	}
}

int EventMgr_EnQ(char* message)
{
    int	data_index = 0;
    int	buff_index = 0;

    eventIndex = (eventIndex + 1)%(EVENT_BUFF_SIZE);

	eventBuff[eventIndex].command = message[buff_index++];
	eventBuff[eventIndex].data_len = message[buff_index++];
	eventBuff[eventIndex].data_len <<= 8;
	eventBuff[eventIndex].data_len |= message[buff_index++];
	eventBuff[eventIndex].data_buff = (uint8_t*) malloc(sizeof(uint8_t)*eventBuff[eventIndex].data_len);

	while(data_index < eventBuff[eventIndex].data_len)
	{
		eventBuff[eventIndex].data_buff[data_index++] = message[buff_index++];
	}

	Report("TCP received command: %i, size: %i\n\r", eventBuff[eventIndex].command, eventBuff[eventIndex].data_len);

	eventCount++;

	return buff_index;
}

void EventMgr_Task (void)
{
//	while(1)
//	{
		if(eventCount)
		{
			EventFire(eventBuff[eventIndex]);
		}
		else
		{
			//yield processor
		}
//	}
}
