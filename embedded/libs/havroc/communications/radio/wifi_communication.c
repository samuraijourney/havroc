#include "havroc/communications/radio/wifi_communication.h"
#include "stdint.h"
#include "havroc/eventmgr/eventmgr.h"
#include <havroc/error.h>

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
static unsigned long g_ulStatus = 0; //SimpleLink Status
static unsigned long g_ulGatewayIP = 0; //Network Gateway IP address
static unsigned char g_ucConnectionSSID[SSID_LEN_MAX + 1]; //Connection SSID
static unsigned char g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
static unsigned long IP_Address = 0;
static char TCP_SendBuffer[BUFF_SIZE];
static int sendIndex = 0;
static char TCP_ReceiveBuffer[BUFF_SIZE];
static long connected_SockID = 0;
static long iSockID = 0;

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
	if (!pWlanEvent) {
		return;
	}

	switch (pWlanEvent->Event) {
	case SL_WLAN_CONNECT_EVENT: {
		SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

		//
		// Information about the connected AP (like name, MAC etc) will be
		// available in 'slWlanConnectAsyncResponse_t'-Applications
		// can use it if required
		//
		//  slWlanConnectAsyncResponse_t *pEventData = NULL;
		// pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
		//

		// Copy new connection SSID and BSSID to global parameters
		memcpy(g_ucConnectionSSID,
				pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_name,
				pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
		memcpy(g_ucConnectionBSSID,
				pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
				SL_BSSID_LENGTH);

		UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
				" BSSID: %x:%x:%x:%x:%x:%x\n\r", g_ucConnectionSSID,
				g_ucConnectionBSSID[0], g_ucConnectionBSSID[1],
				g_ucConnectionBSSID[2], g_ucConnectionBSSID[3],
				g_ucConnectionBSSID[4], g_ucConnectionBSSID[5]);
	}
		break;

	case SL_WLAN_DISCONNECT_EVENT: {
		slWlanConnectAsyncResponse_t* pEventData = NULL;

		CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
		CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

		pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

		// If the user has initiated 'Disconnect' request,
		//'reason_code' is SL_USER_INITIATED_DISCONNECTION
		if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
			UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
					"BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
					g_ucConnectionSSID, g_ucConnectionBSSID[0],
					g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
					g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
					g_ucConnectionBSSID[5]);
		} else {
			UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
					"BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
					g_ucConnectionSSID, g_ucConnectionBSSID[0],
					g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
					g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
					g_ucConnectionBSSID[5]);
		}
		memset(g_ucConnectionSSID, 0, sizeof(g_ucConnectionSSID));
		memset(g_ucConnectionBSSID, 0, sizeof(g_ucConnectionBSSID));
	}
		break;

	default: {
		UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
				pWlanEvent->Event);
	}
		break;
	}
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
	if (!pNetAppEvent) {
		return;
	}

	switch (pNetAppEvent->Event) {
	case SL_NETAPP_IPV4_IPACQUIRED_EVENT: {
		SlIpV4AcquiredAsync_t *pEventData = NULL;

		SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

		//Ip Acquired Event Data
		pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
		IP_Address = pEventData->ip;

		//Gateway IP address
		g_ulGatewayIP = pEventData->gateway;

		UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
				"Gateway=%d.%d.%d.%d\n\r", SL_IPV4_BYTE(IP_Address, 3),
				SL_IPV4_BYTE(IP_Address, 2), SL_IPV4_BYTE(IP_Address, 1),
				SL_IPV4_BYTE(IP_Address, 0), SL_IPV4_BYTE(g_ulGatewayIP, 3),
				SL_IPV4_BYTE(g_ulGatewayIP, 2), SL_IPV4_BYTE(g_ulGatewayIP, 1),
				SL_IPV4_BYTE(g_ulGatewayIP, 0));
	}
		break;

	default: {
		UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
				pNetAppEvent->Event);
	}
		break;
	}
}

//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {
	if (!pDevEvent) {
		return;
	}

	//
	// Most of the general errors are not FATAL are are to be handled
	// appropriately by the application
	//
	UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
			pDevEvent->EventData.deviceEvent.status,
			pDevEvent->EventData.deviceEvent.sender);
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
	if (!pSock) {
		return;
	}

	switch (pSock->Event) {
	case SL_SOCKET_TX_FAILED_EVENT:
		switch (pSock->EventData.status) {
		case SL_ECLOSE:
			UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
					"failed to transmit all queued packets\n\n",
					pSock->EventData.sd);
			break;
		default:
			UART_PRINT("[SOCK ERROR] - TX FAILED : socket %d , reason"
					"(%d) \n\n", pSock->EventData.sd, pSock->EventData.status);
		}
		break;

	default:
		UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n", pSock->Event);
	}

}

//*****************************************************************************
//
//! This function initializes the application variables
//!
//! \param[in]    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables() {
	g_ulStatus = 0;
	g_ulGatewayIP = 0;
	memset(g_ucConnectionSSID, 0, sizeof(g_ucConnectionSSID));
	memset(g_ucConnectionBSSID, 0, sizeof(g_ucConnectionBSSID));
}

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimpleLinkToDefaultState() {
	SlVersionFull ver = { 0 };
	_WlanRxFilterOperationCommandBuff_t RxFilterIdMask = { 0 };

	unsigned char ucConfigOpt = 0;
	unsigned char ucConfigLen = 0;
	unsigned char ucPower = 0;

	long lRetVal = -1;
	long lMode = -1;

	lMode = sl_Start(0, 0, 0);
	ASSERT_ON_ERROR(lMode);

	// If the device is not in station-mode, try configuring it in station-mode
	if (ROLE_STA != lMode) {
		if (ROLE_AP == lMode) {
			// If the device is in AP mode, we need to wait for this event
			// before doing anything
			while (!IS_IP_ACQUIRED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
				_SlNonOsMainLoopTask();
#endif
			}
		}

		// Switch to STA role and restart
		lRetVal = sl_WlanSetMode(ROLE_STA);
		ASSERT_ON_ERROR(lRetVal);

		lRetVal = sl_Stop(0xFF);
		ASSERT_ON_ERROR(lRetVal);

		lRetVal = sl_Start(0, 0, 0);
		ASSERT_ON_ERROR(lRetVal);

		// Check if the device is in station again
		if (ROLE_STA != lRetVal) {
			// We don't want to proceed if the device is not coming up in STA-mode
			return DEVICE_NOT_IN_STATION_MODE;
		}
	}

	// Get the device's version-information
	ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
	ucConfigLen = sizeof(ver);
	lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
			&ucConfigLen, (unsigned char *) (&ver));
	ASSERT_ON_ERROR(lRetVal);

	UART_PRINT("Host Driver Version: %s\n\r", SL_DRIVER_VERSION);
	UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
			ver.NwpVersion[0], ver.NwpVersion[1], ver.NwpVersion[2],
			ver.NwpVersion[3], ver.ChipFwAndPhyVersion.FwVersion[0],
			ver.ChipFwAndPhyVersion.FwVersion[1],
			ver.ChipFwAndPhyVersion.FwVersion[2],
			ver.ChipFwAndPhyVersion.FwVersion[3],
			ver.ChipFwAndPhyVersion.PhyVersion[0],
			ver.ChipFwAndPhyVersion.PhyVersion[1],
			ver.ChipFwAndPhyVersion.PhyVersion[2],
			ver.ChipFwAndPhyVersion.PhyVersion[3]);

	// Set connection policy to Auto + SmartConfig
	//      (Device's default connection policy)
	lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
			SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
	ASSERT_ON_ERROR(lRetVal);

	// Remove all profiles
	lRetVal = sl_WlanProfileDel(0xFF);
	ASSERT_ON_ERROR(lRetVal);

	// Device in station-mode. Disconnect previous connection if any
	// The function returns 0 if 'Disconnected done', negative number if already
	// disconnected Wait for 'disconnection' event if 0 is returned, Ignore
	// other return-codes
	lRetVal = sl_WlanDisconnect();
	if (0 == lRetVal) {
		// Wait
		while (IS_CONNECTED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
			_SlNonOsMainLoopTask();
#endif
		}
	}

	SlNetCfgIpV4Args_t ipV4;
	ipV4.ipV4 = (unsigned long) SL_IPV4_VAL(192, 168, 43, 24); // unsigned long IP  address
	ipV4.ipV4Mask = (unsigned long) SL_IPV4_VAL(255, 255, 255, 0); // unsigned long Subnet mask for this AP/P2P
	ipV4.ipV4Gateway = (unsigned long) SL_IPV4_VAL(10, 0, 0, 1); // unsigned long Default gateway address
	ipV4.ipV4DnsServer = (unsigned long) SL_IPV4_VAL(160, 85, 193, 100); // unsigned long DNS server address
	lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE,
			IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t),
			(unsigned char *) &ipV4);
	ASSERT_ON_ERROR(lRetVal);

	sl_Stop(0);
	sl_Start(NULL, NULL, NULL);

	// Disable scan
	ucConfigOpt = SL_SCAN_POLICY(0);
	lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN, ucConfigOpt, NULL, 0);
	ASSERT_ON_ERROR(lRetVal);

	// Set Tx power level for station mode
	// Number between 0-15, as dB offset from max power - 0 will set max power
	ucPower = 0;
	lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
	WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *) &ucPower);
	ASSERT_ON_ERROR(lRetVal);

	// Set PM policy to normal
	lRetVal = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
	ASSERT_ON_ERROR(lRetVal);

	// Unregister mDNS services
	lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
	ASSERT_ON_ERROR(lRetVal);

	// Remove  all 64 filters (8*8)
	memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
	lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *) &RxFilterIdMask,
			sizeof(_WlanRxFilterOperationCommandBuff_t));
	ASSERT_ON_ERROR(lRetVal);

	lRetVal = sl_Stop(SL_STOP_TIMEOUT);
	ASSERT_ON_ERROR(lRetVal);

	InitializeAppVariables();

	return lRetVal; // Success
}

//****************************************************************************
//
//! \brief Opening a TCP server side socket and receiving data
//!
//! This function opens a TCP socket in Listen mode and waits for an incoming
//!    TCP connection.
//! If a socket connection is established then the function will try to read
//!    1000 TCP packets from the connected client.
//!
//! \param[in] port number on which the server will be listening on
//!
//! \return     0 on success, -1 on error.
//!
//! \note   This function will wait for an incoming connection till
//!                     one is established
//
//****************************************************************************
static int Setup_Socket(unsigned short usPort) {
	SlSockAddrIn_t sAddr;
	SlSockAddrIn_t sLocalAddr;
	int iAddrSize;
	int iStatus;
	long lNonBlocking = 1;

	//filling the TCP server socket address
	sLocalAddr.sin_family = SL_AF_INET;
	sLocalAddr.sin_port = sl_Htons((unsigned short) usPort);
	sLocalAddr.sin_addr.s_addr = 0;

	// creating a TCP socket
	iSockID = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
	if (iSockID < 0) {
		// error
		ASSERT_ON_ERROR(TCP_SERVER_FAILED);
	}

	iAddrSize = sizeof(SlSockAddrIn_t);

	// binding the TCP socket to the TCP server address
	iStatus = sl_Bind(iSockID, (SlSockAddr_t *) &sLocalAddr, iAddrSize);
	if (iStatus < 0) {
		// error
		ASSERT_ON_ERROR(sl_Close(iSockID));
		ASSERT_ON_ERROR(TCP_SERVER_FAILED);
	}

	// putting the socket for listening to the incoming TCP connection
	iStatus = sl_Listen(iSockID, 0);
	if (iStatus < 0) {
		ASSERT_ON_ERROR(sl_Close(iSockID));
		ASSERT_ON_ERROR(TCP_SERVER_FAILED);
	}

	// setting socket option to make the socket as non blocking
	iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
			&lNonBlocking, sizeof(lNonBlocking));
	if (iStatus < 0) {
		ASSERT_ON_ERROR(sl_Close(iSockID));
		ASSERT_ON_ERROR(TCP_SERVER_FAILED);
	}
	connected_SockID = SL_EAGAIN;

	// waiting for an incoming TCP connection
	while (connected_SockID < 0) {
		// accepts a connection form a TCP client, if there is any
		// otherwise returns SL_EAGAIN
		connected_SockID = sl_Accept(iSockID, (struct SlSockAddr_t *) &sAddr,
				(SlSocklen_t*) &iAddrSize);
		if (connected_SockID == SL_EAGAIN) {
			MAP_UtilsDelay(10000);
		} else if (connected_SockID < 0) {
			// error
			ASSERT_ON_ERROR(sl_Close(connected_SockID));
			ASSERT_ON_ERROR(sl_Close(iSockID));
			ASSERT_ON_ERROR(TCP_SERVER_FAILED);
		}
	}

	return connected_SockID;
}

static void WiFiRun(UArg arg0, UArg arg1)
{
	int iStatus;
	int buff_index = 0;

	while (1)
	{
		iStatus = sl_Recv(connected_SockID, TCP_ReceiveBuffer, BUFF_SIZE, 0);
		while (iStatus <= 0)
		{
			// error, keep restarting
			WlanOff();
			iStatus = WlanStartup();
		}

		buff_index = 0;

		while (buff_index < BUFF_SIZE)
		{
			if (TCP_ReceiveBuffer[buff_index++] == SYNC_START_CODE_BYTE)
			{
				buff_index += EventEnQ(&TCP_ReceiveBuffer[buff_index]);
			}
		}

		WiFiSend();
	}
}

int WiFiSendEnQ(sendMessage message)
{
	char temp = SYNC_START_CODE_BYTE;
	if((sendIndex + 4*message.length + 3) < BUFF_SIZE)
	{
		memcpy(&TCP_SendBuffer[sendIndex++], &(temp), sizeof temp);
		memcpy(&TCP_SendBuffer[sendIndex++], &(message.command), sizeof message.command);
		memcpy(&TCP_SendBuffer[sendIndex++], &(message.length), sizeof message.length);
		memcpy(&TCP_SendBuffer[sendIndex], message.data, (sizeof *(message.data))*message.length);

		sendIndex += 4*message.length;
	}
	else
	{
		return WiFi_BUFFER_FULL;
	}

	return SUCCESS;
}

static void WiFiSend()
{
	int iStatus;
	long lLoopCount = 0;

	// sending multiple packets to the TCP server
	while (lLoopCount < TCP_PACKET_COUNT)
	{
		// sending packet

		iStatus = sl_Send(connected_SockID, TCP_SendBuffer, BUFF_SIZE, 0);
		while (iStatus <= 0)
		{
			// error
			WlanOff();
			iStatus = WlanStartup();

			if (iStatus > 0)
			{
				lLoopCount--;
			}
		}

		lLoopCount++;
	}

	sendIndex = 0;

	Report("TCP sent %i bytes successful\n\r", iStatus);
}

//****************************************************************************
//
//!  \brief Connecting to a WLAN Accesspoint
//!
//!   This function connects to the required AP (SSID_NAME) with Security
//!   parameters specified in te form of macros at the top of this file
//!
//!   \param[in]              None
//!
//!   \return       status value
//!
//!   \warning    If the WLAN connection fails or we don't aquire an IP
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
static long WlanConnect()
{
	long lRetVal = 0;

	lRetVal = sl_WlanConnect((signed char*) SSID_NAME, strlen(SSID_NAME), 0, 0,
			0);
	ASSERT_ON_ERROR(lRetVal);

	while ((!IS_CONNECTED(g_ulStatus)))
	{
		// Wait for WLAN Event
#ifndef SL_PLATFORM_MULTI_THREADED
		_SlNonOsMainLoopTask();
#endif
	}

	return SUCCESS;
}

static int WlanStartup()
{
	if(WlanInit() != 0)
	{
		UART_PRINT("Failed to start WiFi radio \n\r");
		return WiFi_START_FAIL;
	}

	UART_PRINT("Connecting to AP: %s ...\r\n", SSID_NAME);

	// Connecting to WLAN AP
	if (WlanConnect() < 0)
	{
		UART_PRINT("Failed to establish connection w/ an AP \n\r");
		return WiFi_START_FAIL;
	}

	UART_PRINT("Connected to AP: %s \n\r", SSID_NAME);

	UART_PRINT("Device IP: %d.%d.%d.%d\n\r\n\r", SL_IPV4_BYTE(IP_Address, 3),
			SL_IPV4_BYTE(IP_Address, 2), SL_IPV4_BYTE(IP_Address, 1),
			SL_IPV4_BYTE(IP_Address, 0));

	if (Setup_Socket(PORT_NUM_TCP) < 0)
	{
		UART_PRINT("TCP socket setup failed\n\r");
		return WiFi_START_FAIL;
	}

	Task_Handle task0;

	task0 = Task_create((Task_FuncPtr)WiFiRun, NULL, NULL);
	if (task0 == NULL) {
		return WiFi_START_FAIL;
	}

	return SUCCESS;
}

static int WlanOff()
{
	// close the connected socket after receiving from connected TCP client
	ASSERT_ON_ERROR(sl_Close(connected_SockID));

	// close the listening socket
	ASSERT_ON_ERROR(sl_Close(iSockID));

	Report("TCP Sockets closed\n\r");

	sl_WlanDisconnect();

	// power off the network processor
	sl_Stop(SL_STOP_TIMEOUT);

	return SUCCESS;
}

static int WlanInit()
{
	long lRetVal = -1;

	InitializeAppVariables();

	// Following function configure the device to default state by cleaning
	// the persistent settings stored in NVMEM

	lRetVal = ConfigureSimpleLinkToDefaultState();

	if (lRetVal < 0)
	{
		if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
			UART_PRINT("Failed to configure the device in its default state \n\r");

		return WiFi_START_FAIL;
	}

	UART_PRINT("Device is configured in default state \n\r");

	lRetVal = sl_Start(0, 0, 0);
	if (lRetVal < 0 || lRetVal != ROLE_STA)
	{
		UART_PRINT("Failed to start the device \n\r");
		return WiFi_START_FAIL;
	}

	UART_PRINT("Device started as STATION \n\r");

	return SUCCESS;
}

