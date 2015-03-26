#include "stdbool.h"
#include "uart_if.h"

#include "havroc/communications/suit/suit_net_manager.h"
#include "havroc/communications/suit/suit_i2c.h"
#include "havroc/eventmgr/eventmgr.h"
#include "havroc/actuation/motor.h"
#include "havroc/command.h"
#include "havroc/havroc_utils/havrocutils.h"

#include "havroc/communications/suit/suit_net_map.h"

#define SELECTED_SNODE_NONE		0xFF

/*STATIC FXN DEFINITIONS*/
static void motorEventCallback(event currEvent);
static void processMotorDataCmd(uint8_t* data, uint16_t length);

extern SuitNet_SuperNode superNodes[NUM_SUPER_NODES];
extern SuitNet_Node nodes[NUM_NODES];

uint8_t currentSelectedSuperNode = SELECTED_SNODE_NONE;

static void motorEventCallback(event currEvent)
{
	switch(currEvent.command)
	{
		case MOTOR_DATA_CMD:
			processMotorDataCmd(currEvent.data_buff, currEvent.data_len);
			break;
		default:
			break;
	}
}

static void processMotorDataCmd(uint8_t* data, uint16_t length)
{
	uint8_t motorId;
	uint16_t i;

	for (i = 0; i < length; i+=3)
	{
		Report("Running motor %d\n\r", motorId);
		//get motor node's index
		motorId = MOTOR_NODE_START_INDEX + data[i];

		//do supernode select
		suitNetManager_nodeSelect(motorId);
		motor_runWaveform();
	}
}

void suitNetManager_boardTest(uint8_t muxAddr, uint8_t driverTestCount)
{
	uint8_t writeBuff[1];
	uint8_t readBuff[1];
	int i;

	writeBuff[0] = 0x02;
	Report("Mux Test --> Verdict: ");

	if (suit_i2c_transfer(muxAddr, writeBuff, 1, NULL, 0) != 0)
		Report("FAIL!\n\r");
	else
		Report("PASS!\n\r");

	for (i = 0; i < driverTestCount; i++)
	{
		//switch mux
		writeBuff[0] = 1 << i;
		suit_i2c_transfer(muxAddr, writeBuff, 1, NULL, 0);
		readBuff[0] = 0;
 		suit_i2c_read(0x5A, 0x00, readBuff, 1);
		Report("Motor %i Test --> Read: 0x%02x Verdict: ", i, readBuff[0]);
		if (readBuff[0] == 0xC0)
			Report("PASS!\n\r");
		else
			Report("FAIL!\n\r");

		delay(100);
	}
}

void suitNetManager_boardMotorTest(uint8_t id)
{
	uint8_t writeBuff[1];

	//switch mux
	writeBuff[0] = 1 << id;
	suit_i2c_transfer(0x77, writeBuff, 1, NULL, 0);

	motor_init();
	motor_runWaveform();
}

SuitNetErrorCode suitNetManager_imu_i2c_transfer(uint8_t nodeIndex,
											bool isMagnetometer,
											uint8_t writeBuff[],
											size_t writeCount,
											uint8_t readBuff[],
											size_t readCount)
{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;

	if (nodeIndex < NUM_NODES)
	{	
		if ((retVal = suitNetManager_nodeSelect(nodeIndex)) == SUITNET_E_SUCCESS)
		{
			uint8_t addr = (isMagnetometer) ? IMU_MAG_ADDR: IMU_MPU_ADDR;
			if (!suit_i2c_transfer(addr, writeBuff, writeCount,
								  readBuff, readCount))
			{
				retVal = SUITNET_E_I2C_ERROR;
			}
		}
	}

	return retVal;
}

SuitNetErrorCode suitNetManager_imu_i2c_read(uint8_t nodeIndex,	
											bool isMagnetometer,
											uint8_t regAddr,
											uint8_t readBuff[],
											size_t readCount)

{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;

	if (nodeIndex < NUM_NODES)
	{	
		if ((retVal = suitNetManager_nodeSelect(nodeIndex)) == SUITNET_E_SUCCESS)
		{
			uint8_t addr = (isMagnetometer) ? IMU_MAG_ADDR: IMU_MPU_ADDR;
			if (suit_i2c_read(addr, regAddr, readBuff, readCount) != 0)
			{
				retVal = SUITNET_E_I2C_ERROR;
			}
		}
	}

	return retVal;
}

SuitNetErrorCode suitNetManager_imu_i2c_write(uint8_t nodeIndex,
											bool isMagnetometer,
											uint8_t regAddr,
											uint8_t writeBuff[],
											size_t writeCount)

{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;

	if (nodeIndex < NUM_NODES)
	{	
		if ((retVal = suitNetManager_nodeSelect(nodeIndex)) == SUITNET_E_SUCCESS)
		{
			uint8_t addr = (isMagnetometer) ? IMU_MAG_ADDR: IMU_MPU_ADDR;
			if (suit_i2c_write(addr, regAddr, writeBuff, writeCount) != 0)
			{
				retVal = SUITNET_E_I2C_ERROR;
			}
		}
	}

	return retVal;
}

SuitNetErrorCode suitNetManager_nodeSelect(uint8_t nodeIndex)
{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;
	uint8_t writeBuff[1];
	SuitNet_Node *node = &nodes[nodeIndex];

	//if supernode -1, bypass node selection
	if (node->superNodeId >= 0)
	{
		//deselect current supernode
		if (currentSelectedSuperNode != node->superNodeId)
		{
			suitNetManager_clearNodeSelect(node);
		}

		//get supernode object from array
		SuitNet_SuperNode *superNode = superNodes + node->superNodeId;

		//select primary mux line
		if (superNode->primaryMuxLine >= 0)
		{
			writeBuff[0] = 1 << superNode->primaryMuxLine;
			if (suit_i2c_transfer(PRIMARY_MUX_ADDR, writeBuff, 1, 0, 0)
															!= SUIT_I2C_E_SUCCESS)
			{
				retVal = SUITNET_E_I2C_ERROR;
				superNode->status = SUITNET_SNODE_STAT_I2C_ERROR;
			}
		}

		//select supernode
		if (retVal == SUITNET_E_SUCCESS)
		{
			writeBuff[0] = 1 << node->muxChannel;
			if (suit_i2c_transfer(superNode->addr, writeBuff, 1, 0, 0)
																!= SUIT_I2C_E_SUCCESS)
			{
				retVal = SUITNET_E_I2C_ERROR;
				superNode->status = SUITNET_SNODE_STAT_I2C_ERROR;
			}
			else
			{
				superNode->status = SUITNET_SNODE_STAT_OK;
				currentSelectedSuperNode = node->superNodeId;
			}
		}
	}

	return retVal;
}

SuitNetErrorCode suitNetManager_clearNodeSelect(SuitNet_Node *node)
{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;
	uint8_t writeBuff[1];
	
	if (node->superNodeId < NUM_SUPER_NODES)
	{
		//get supernode object from array
		SuitNet_SuperNode *superNode = superNodes + node->superNodeId;

		//select primary mux line
		if (superNode->primaryMuxLine >= 0)
		{	
			writeBuff[0] = 1 << superNode->primaryMuxLine;
			if (suit_i2c_transfer(PRIMARY_MUX_ADDR, writeBuff, 1, 0, 0) 
															!= SUIT_I2C_E_SUCCESS)
			{
				retVal = SUITNET_E_I2C_ERROR;
				superNode->status = SUITNET_SNODE_STAT_I2C_ERROR;
			}
		}

		//deselect supernode
		if (retVal == SUITNET_E_SUCCESS)
		{
			writeBuff[0] = 0;
			if (suit_i2c_transfer(superNode->addr, writeBuff, 1, 0, 0) 
															!= SUIT_I2C_E_SUCCESS)
			{
				retVal = SUITNET_E_I2C_ERROR;
				superNode->status = SUITNET_SNODE_STAT_I2C_ERROR;
			}
			else 
			{
				superNode->status = SUITNET_SNODE_STAT_OK;
				currentSelectedSuperNode = SELECTED_SNODE_NONE;
			}
		}
	}
	else
	{
		retVal = SUITNET_E_FAIL;
	}

	return retVal;
}

SuitNetErrorCode suitNetManager_resetSelect() 
{
	int i;
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;

	//iteratively deselect all supernodes
	for (i = 0; i < NUM_SUPER_NODES; i++)
	{
		retVal = suitNetManager_clearNodeSelect(nodes + i);
	}

	return retVal;
}

SuitNetErrorCode suitNetManager_init()
{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;

	currentSelectedSuperNode = SELECTED_SNODE_NONE;
	EventRegisterCB(MOTOR_MOD, motorEventCallback);

	return retVal;
}
