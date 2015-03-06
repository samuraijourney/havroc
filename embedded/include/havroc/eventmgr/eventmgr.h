/*
 * eventmgr.h
 *
 *  Created on: Feb 3, 2015
 *      Author: Akram
 */

#ifndef EVENTMGR_H_
#define EVENTMGR_H_

#if defined(CCWARE) || defined(TIVAWARE)
#include <ti/sysbios/knl/Task.h>
#endif

#include "stdint.h"

#define EVENT_BUFF_SIZE 1000
#define MAX_CALLBACKS   10


typedef struct _event
{
    int8_t 	    command;
    uint16_t 	data_len;
    uint8_t*    data_buff;
} event;

typedef void(*EVENT_CB)(event currEvent);

int EventRegisterCB(int32_t command, EVENT_CB Callback);
static int EventFire(event currEvent);
int EventEnQ(char* message);
int EventStart();
void EventRun (UArg arg0, UArg arg1);

#endif /* EVENTMGR_H_ */
