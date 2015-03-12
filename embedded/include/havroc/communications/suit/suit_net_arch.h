#ifndef SUIT_NET_ARCH_H
#define SUIT_NET_ARCH_H

#include "suit_net.h"

//motors list
Motor motors[NUM_MOTORS] = {
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0},
};

//supernode init
SuitNet_SuperNode superNodes[NUM_SUPER_NODES] = {
	{0, MUX_INACTIVE, SUITNET_SNODE_STAT_UNKNOWN, &(motors[0]), 3, MUX_ADDR | 0x1}, //0
	{0, MUX_INACTIVE, SUITNET_SNODE_STAT_UNKNOWN, &(motors[3]), 2, MUX_ADDR | 0x2} //1
};

//nodes init
SuitNet_Node nodes[NUM_NODES] = {
	{SUITNET_N_MOTOR, 0, &(motors[0])},
	{SUITNET_N_MOTOR, 0, &(motors[1])},
	{SUITNET_N_MOTOR, 0, &(motors[2])},
	{SUITNET_N_MOTOR, 1, &(motors[3])},
	{SUITNET_N_MOTOR, 1, &(motors[4])}
};



#endif
