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
#include "pin_mux_config.h"
#include <ti/sysbios/BIOS.h>

#define IP_ADDR           			0xc0a8006E /* 192.168.0.110 */
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
	char module;
	char command;
	char length_high;
	char length_low;
	float* data;
} sendMessage;

//****************************************************************************
//                       FUNCTION PROTOTYPES
//****************************************************************************
int WiFiSendEnQ(sendMessage message);
int WiFiStartup();
void WlanStartTask();
void WiFiRun(UArg arg0, UArg arg1);
bool isWiFiActive();
static void WiFiSend();
static int Setup_Socket(unsigned short usPort);
static int WlanInit();
static int  WlanOff();
static long WlanConnect();
static void InitializeAppVariables();
static long ConfigureSimpleLinkToDefaultState();


#endif /* WIFI_COMMUNICATION_H_ */
