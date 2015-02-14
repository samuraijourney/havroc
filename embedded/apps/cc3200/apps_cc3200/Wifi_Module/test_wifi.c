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
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include <Board.h>
#include "havroc/communications/radio/wifi_communication.h"
#include "havroc/eventmgr/eventmgr.h"

extern unsigned long  		  IP_Address;
extern unsigned long  		  UDP_BufferSize;
extern unsigned long  		  sendCount;
extern unsigned long  		  UDP_Buffer[BUFF_SIZE];
extern long  		  		  TCP_SendBuffer[BUFF_SIZE];

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void)
{
    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

void test_callback(event current_event)
{
	int i = current_event.command;
}


// Test Application
void testFxn()
{
    long lRetVal = -1;

	// Board Initialization
	BoardInit();

	// Initialize the uDMA
	UDMAInit();

	// Configure the pinmux settings for the peripherals exercised
	PinMuxConfig();

	// Configuring UART
	InitTerm();

    EventRegisterCB(TRACKING_CMD, &(test_callback));

    WlanStartup();

//    ip_address[0]= SL_IPV4_BYTE(IP_Address,3);
//    ip_address[1]= SL_IPV4_BYTE(IP_Address,2);
//    ip_address[2]= SL_IPV4_BYTE(IP_Address,1);
//    ip_address[3]= SL_IPV4_BYTE(IP_Address,0);
//
//    short i = 0;
//
//    for(i = 0; i < 4; i++)
//    {
//    	UDP_Buffer[i] = ip_address[i];
//    }
//
//    UDP_BufferSize = 4;
//
//	// Run UDP client to broadcast IP address
//    lRetVal = UDP_Broadcast(PORT_NUM_UDP);
//    if(lRetVal < 0)
//    {
//        ERR_PRINT(lRetVal);
//        LOOP_FOREVER();
//    }

//	if(lRetVal < 0)
//	{
//		UART_PRINT("TCP send command failed\n\r");
//		LOOP_FOREVER();
//	}

    lRetVal = TCP_Receive();

    if(lRetVal < 0)
   	{
   		UART_PRINT("TCP receive command failed\n\r");
   		LOOP_FOREVER();
   	}

    lRetVal = WlanOff();

	if(lRetVal < 0)
	{
		UART_PRINT("Wlan turn off command failed\n\r");
		LOOP_FOREVER();
	}
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Handle task0;

	task0 = Task_create((Task_FuncPtr)testFxn, NULL, NULL);
	if (task0 == NULL) {
		System_printf("Task create failed.\n");
		System_flush();
	}

	/* Call board init functions. */
	Board_initGeneral();

    /* Start BIOS */
    BIOS_start();

    return (0);
}



