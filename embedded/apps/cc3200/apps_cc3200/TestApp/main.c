/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Timestamp.h>


/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* Example/Board Header files */
#include <Board.h>

/* HaVRoC library files */
#include "havroc/communications/radio/wifi_communication.h"
#include "havroc/eventmgr/eventmgr.h"
#include "havroc/tracking/service.h"

#include "uart_if.h"
#include "common.h"

Event_Handle   EventMgr_Event;
Event_Handle   Timer_Event;

Timer_Params   timerParams;
Timer_Handle   myTimer;

#define 	   EventReceived 	1
#define 	   Passed_5ms 		2

unsigned long millis_main ()
{
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	return (Timestamp_get32()*1000.0/(1.0*freq.lo));
}

static void BoardInit(void)
{
    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

void TimerISR()
{
	Event_post(Timer_Event, Passed_5ms);
}

void Fxn1()
{
	while(1)
	{
		System_printf("This is Task 1\n");
		System_flush();

		Event_post(EventMgr_Event, EventReceived);

		Task_yield();
	}
}

void Fxn2()
{
	UInt events;

	while(1)
	{
		System_printf("This is Task 2 - Before Check\n");
		System_flush();
		events = Event_pend(EventMgr_Event, EventReceived, Event_Id_NONE, BIOS_WAIT_FOREVER);
		if(events & EventReceived)
		{
			System_printf("This is Task 2 - After Check\n");
			System_flush();
		}

		Task_yield();
	}
}

void Fxn3()
{
	float prev = 0;
	float now = 0;
	UInt events;

	while(1)
	{
		events = Event_pend(Timer_Event, Passed_5ms, Event_Id_NONE, BIOS_WAIT_FOREVER);

		if(events & Passed_5ms)
		{
			now = millis_main();

			System_printf("This is Task 3 - Elapsed Time is %.04f\n", now-prev);
			System_flush();

			prev = now;
		}

		Task_yield();
	}
}
int main(void)
{
//	Clock_Params   clockParams;
//	Clock_Handle   clkHandle;

    Task_Handle task1;
	Task_Handle task2;
	Task_Handle task3;

	Task_Params    params;

	/* Call board init functions. */
	Board_initGeneral();

    // Board Initialization
	BoardInit();

	// Initialize the uDMA
	UDMAInit();

	// Configuring UART
	InitTerm();

	// Initialize I2C
    Board_initI2C();

	EventMgr_Event = Event_create(NULL, NULL);
	Timer_Event = Event_create(NULL, NULL);

	Task_Params_init(&params);
	params.priority = 10;

	//WlanStartTask();

	task1 = Task_create((Task_FuncPtr)Fxn1, &params, NULL);
	if (task1 == NULL) {
		System_printf("Task create failed.\n");
		System_flush();
	}

	task2 = Task_create((Task_FuncPtr)Fxn2, &params, NULL);
	if (task2 == NULL) {
		System_printf("Task create failed.\n");
		System_flush();
	}

	params.priority = 15;

	task3 = Task_create((Task_FuncPtr)Fxn3, &params, NULL);
	if (task3 == NULL) {
		System_printf("Task create failed.\n");
		System_flush();
	}

    /* Start BIOS */
    BIOS_start();
   // Timer_start(myTimer);

    return (0);
}
