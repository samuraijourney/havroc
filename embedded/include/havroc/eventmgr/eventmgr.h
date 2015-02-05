/*
 * eventmgr.h
 *
 *  Created on: Feb 3, 2015
 *      Author: Akram
 */

#ifndef EVENTMGR_H_
#define EVENTMGR_H_

#include "stdint.h"

#define EVENT_BUFF_SIZE 1000
#define MAX_CALLBACKS   10


typedef struct _event
{
    uint8_t 	command;
    uint16_t 	data_len;
    uint8_t*    data_buff;
} event;

typedef void(*EVENT_CB)(event currEvent);

void EventRegisterCB(uint32_t command, EVENT_CB Callback);
static void EventFire(event currEvent);
int EventMgr_EnQ(char* message);
void EventMgr_Task (void);

#endif /* EVENTMGR_H_ */
