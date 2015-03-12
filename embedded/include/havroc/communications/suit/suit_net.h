#ifndef SUIT_NET_H_
#define SUIT_NET_H_

#include "havroc/actuation/motor.h"

#define MUX_ADDR			0x70
#define MUX_INACTIVE		-1

#define NUM_SUPER_NODES		2
#define NUM_NODES 			5
#define NUM_MOTORS			NUM_NODES

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
	SUITNET_E_I2C_ERROR
} SuitNetErrorCode;

/*
 * Enum node types
 */
typedef enum _SuitNetNodeType
{
	SUITNET_N_MOTOR = 0,
	SUITNET_N_UNKNOWN
} SuitNetNodeType;

/*
 * Suitnet node struct
 */
typedef struct _SuitNet_Node
{
	//node type: motor, other
	SuitNetNodeType nodeType;
	//supernodeId which this node belongs to
	uint8_t superNodeId;
	//Motor object of node
	Motor *motor;
} SuitNet_Node;

/*
 * Suitnet supernode struct
 */
typedef struct _SuitNet_SuperNode
{	
	//last muxed channel of supernode; -1 if inactive
	int8_t muxedChannel;
	//keeps status of supernode: active or irresponsive
	SuitNetSuperNodeStatus status;
	//list of nodes
	SuitNet_Node *nodes;
	//count of nodes
	uint8_t nodesCount;
	//supernode address
	uint8_t addr;
} SuitNet_SuperNode;

extern SuitNet_SuperNode superNodes[NUM_SUPER_NODES];
extern SuitNet_Node nodes[NUM_NODES];
extern Motor motors[NUM_MOTORS];

//Selects supernode and opens channel
SuitNetErrorCode suitnet_superNodeSelect(uint8_t superNodeId, uint8_t channel);
//Deselects supernode
SuitNetErrorCode suitnet_clearSuperNodeSelect(uint8_t superNodeId);
//Deselects all supernodes
SuitNetErrorCode suitnet_resetAllSuperNodes();

#endif /* SUIT_NET_H_ */
