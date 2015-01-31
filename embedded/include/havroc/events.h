/*
 * events.h
 *
 *  Created on: Jan 29, 2015
 *      Author: Akram
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include "stdint.h"

typedef struct _event
{
    uint8_t 	command;
    uint16_t 	data_len;
    uint8_t*    data_buff;
} event;


#endif /* EVENTS_H_ */
