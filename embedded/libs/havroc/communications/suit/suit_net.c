#include "havroc/communications/suit/suit_net.h"

#define SELECTED_SNODE_NONE		0xFF

uint8_t currentSelectedSuperNode = SELECTED_SNODE_NONE;

SuitNetErrorCode suitnet_superNodeSelect(uint8_t superNodeId, uint8_t channel)
{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;
	uint8_t writeBuff[1];
	writeBuff[0] = 0x1 << channel;

	//deselect current supernode
	if (currentSelectedSuperNode != superNodeId)
	{
		suitnet_clearSuperNodeSelect(superNodeId);
	}
	
	//select supernode
	if (suit_i2c_transfer(superNodes[superNodeId].addr, writeBuff, 1, 0, 0) 
														!= SUIT_I2C_E_SUCCESS)
	{
		retVal = SUITNET_E_I2C_ERROR;
		superNodes[superNodeId].status = SUITNET_SNODE_STAT_I2C_ERROR;
	}
	else
	{
		superNodes[superNodeId].status = SUITNET_SNODE_STAT_OK;
		superNodes[superNodeId].muxedChannel = channel;
		currentSelectedSuperNode = superNodeId;
	}

	return retVal;
}

SuitNetErrorCode suitnet_clearSuperNodeSelect(uint8_t superNodeId)
{
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;
	uint8_t writeBuff[1] = {0};
	
	//deselect supernode
	if (suit_i2c_transfer(superNodes[superNodeId].addr, writeBuff, 1, 0, 0) 
													!= SUIT_I2C_E_SUCCESS)
	{
		retVal = SUITNET_E_I2C_ERROR;
		superNodes[superNodeId].status = SUITNET_SNODE_STAT_I2C_ERROR;
	}
	else 
	{
		superNodes[superNodeId].status = SUITNET_SNODE_STAT_OK;
		superNodes[superNodeId].muxedChannel = MUX_INACTIVE;
		currentSelectedSuperNode = SELECTED_SNODE_NONE;
	}

	return retVal;
}

SuitNetErrorCode suitnet_resetAllSuperNodes() 
{
	int i;
	SuitNetErrorCode retVal = SUITNET_E_SUCCESS;

	//iteratively deselect all supernodes
	for (i = 0; i < NUM_SUPER_NODES; i++)
	{
		retVal = suitnet_clearSuperNodeSelect(i);
	}

	return retVal;
}
