/*
 * wifi_communication.h
 *
 *  Created on: Jan 27, 2015
 *      Author: Akram
 */

#ifndef WIFI_COMMUNICATION_H_
#define WIFI_COMMUNICATION_H_

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

// Standard includes
#include <stdlib.h>
#include <string.h>
#include "stdbool.h"

// simplelink includes
#include "simplelink.h"
#include "wlan.h"

// driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"

// common interface includes
#include "udma_if.h"
#include "common.h"
#include "pin_mux_config.h"

/* HaVRoc Library Includes */
#include "havroc/command.h"

#define PORT_NUM_TCP        		13
#define BUFF_SIZE	   	   			1000
#define TCP_PACKET_COUNT   			1
#define SYNC_START_CODE_BYTE		0xFF

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    TCP_SERVER_FAILED = -0x7D0,
    DEVICE_NOT_IN_STATION_MODE = TCP_SERVER_FAILED - 1,
    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

typedef struct _sendMessage
{
	uint8_t 	module;
	uint8_t 	command;
	uint16_t 	length;
	uint8_t 	arm;
	float	    data[6];
} sendMessage;

//****************************************************************************
//                       FUNCTION PROTOTYPES
//****************************************************************************
int WiFiSendEnQ(sendMessage message);
int WiFiStartup(uint8_t board_arm);
bool isWiFiActive();
int WiFiSend();
int WiFiReceive();
int WlanOff();
static int Setup_Socket(unsigned short usPort);
static int WlanInit();
static long WlanConnect();
static void InitializeAppVariables();
static long ConfigureSimpleLinkToDefaultState();


#endif /* WIFI_COMMUNICATION_H_ */
