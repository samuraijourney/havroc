/*
 * wifi_communication.h
 *
 *  Created on: Jan 27, 2015
 *      Author: Akram
 */

#ifndef WIFI_COMMUNICATION_H_
#define WIFI_COMMUNICATION_H_

// Standard includes
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
#ifndef NOTERM
#include "uart_if.h"
#endif

#include "havroc/command.h"
#include "havroc/events.h"

#define IP_ADDR           			0xc0a8006E /* 192.168.0.110 */
#define PORT_NUM_UDP        		8888
#define PORT_NUM_TCP        		13
#define BUFF_SIZE	   	   			1000
#define UDP_PACKET_COUNT   			1000
#define TCP_PACKET_COUNT   			1
#define SYNC_START_CODE_BYTE		0xFF
#define SYNC_END_CODE_BYTE			0xF0

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    UDP_CLIENT_FAILED = -0x7D0,
    TCP_SERVER_FAILED = UDP_CLIENT_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = UDP_CLIENT_FAILED - 2,
    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

//****************************************************************************
//                       FUNCTION PROTOTYPES
//****************************************************************************
int UDP_Broadcast(unsigned short usPort);
int TCP_Receive();
int TCP_Send();
int Setup_Socket(unsigned short usPort);
void WlanInit();
int  WlanOff();
long WlanConnect();
static void BoardInit();
static void InitializeAppVariables();
static long ConfigureSimpleLinkToDefaultState();


#endif /* WIFI_COMMUNICATION_H_ */
