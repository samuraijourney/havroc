/*
 * test_app.c
 *
 *  Created on: Jan 27, 2015
 *      Author: Akram
 */


#include "havroc/communications/radio/wifi_communication.h"
#include "havroc/events.h"
#include "pin_mux_config.h"


extern unsigned long  		  IP_Address;
extern unsigned long  		  UDP_BufferSize;
extern unsigned long  		  TCP_BufferSize;
extern unsigned long  		  UDP_Buffer[BUFF_SIZE];
extern long  		  		  TCP_SendBuffer[BUFF_SIZE];
event						  eventBuff[BUFF_SIZE];
int		      				  eventIndex;

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


// Test Application
void main()
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

	// Init Wifi for UDP operation
    WlanInit();

    UART_PRINT("Connecting to AP: %s ...\r\n",SSID_NAME);

    // Connecting to WLAN AP
    lRetVal = WlanConnect();
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to establish connection w/ an AP \n\r");
        LOOP_FOREVER();
    }

    UART_PRINT("Connected to AP: %s \n\r",SSID_NAME);

    UART_PRINT("Device IP: %d.%d.%d.%d\n\r\n\r",
                      SL_IPV4_BYTE(IP_Address,3),
                      SL_IPV4_BYTE(IP_Address,2),
                      SL_IPV4_BYTE(IP_Address,1),
                      SL_IPV4_BYTE(IP_Address,0));

    lRetVal = Setup_Socket(PORT_NUM_TCP);

    if(lRetVal < 0)
	{
		UART_PRINT("TCP socket setup failed\n\r");
		LOOP_FOREVER();
	}

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

    while (1)
    {
        _SlNonOsMainLoopTask();
    }
}




