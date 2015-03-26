#ifndef SUIT_NET_MANAGER_H_
#define SUIT_NET_MANAGER_H_

#include "havroc/actuation/motor.h"

/*
 * Enum statuses of supernodes
 */
typedef enum _SuitNetSuperNodeStatus
{
	SUITNET_SNODE_STAT_OK = 0,
	SUITNET_SNODE_STAT_I2C_ERROR = 1,
	SUITNET_SNODE_STAT_UNKNOWN
} SuitNetSuperNodeStatus;

/*
 * Enum suitnet error codes
 */
typedef enum _SuitNetErrorCode
{
	SUITNET_E_SUCCESS = 0,
	SUITNET_E_I2C_ERROR,
	SUITNET_E_FAIL
} SuitNetErrorCode;

/*
 * Enum node types
 */
typedef enum _SuitNetNodeType
{
	SUITNET_N_MOTOR = 0,
	SUITNET_N_IMU,
	SUITNET_N_UNKNOWN
} SuitNetNodeType;

/*
 * Suitnet node struct
 */
typedef struct _SuitNet_Node
{
	//node type: motor, IMU
	SuitNetNodeType nodeType;
	//supernodeId which this node belongs to
	int8_t superNodeId;
	//mux channel of this node
	uint8_t muxChannel;
} SuitNet_Node;

/*
 * Suitnet supernode struct
 */
typedef struct _SuitNet_SuperNode
{	
	//primary mux line that supernode is on; -1 to bypass primary mux
	int8_t primaryMuxLine;
	//keeps status of supernode: active or irresponsive
	SuitNetSuperNodeStatus status;
	//list of nodes
	SuitNet_Node *nodes;
	//count of nodes
	uint8_t nodesCount;
	//supernode address
	uint8_t addr;
} SuitNet_SuperNode;

//Selects supernode and opens channel
SuitNetErrorCode suitNetManager_nodeSelect(uint8_t nodeIndex);
//Deselects supernode
SuitNetErrorCode suitNetManager_clearNodeSelect(SuitNet_Node *node);
//Deselects all supernodes
SuitNetErrorCode suitNetManager_resetSelect();

SuitNetErrorCode suitNetManager_imu_i2c_transfer(uint8_t nodeIndex,
											bool isMagnetometer,
											uint8_t writeBuff[],
											size_t writeCount,
											uint8_t readBuff[],
											size_t readCount);
SuitNetErrorCode suitNetManager_imu_i2c_read(uint8_t nodeIndex,
											bool isMagnetometer,
											uint8_t regAddr,
											uint8_t readBuff[],
											size_t readCount);
SuitNetErrorCode suitNetManager_imu_i2c_write(uint8_t nodeIndex,
											bool isMagnetometer,
											uint8_t regAddr,
											uint8_t writeBuff[],
											size_t writeCount);
/*Board diag test*/
void suitNetManager_boardTest(uint8_t muxAddr, uint8_t driverTestCount);
void suitNetManager_boardMotorTest(uint8_t id);
#endif /* SUIT_NET_H_ */
