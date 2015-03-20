#ifndef SUIT_NET_ARCH_H
#define SUIT_NET_ARCH_H

#include "havroc/communications/suit/suit_net_manager.h"
#include "havroc/actuation/motor.h"

#define IMU_MPU_ADDR			0x68
#define IMU_MAG_ADDR			0x0C

#define MUX_BASE_ADDR			0x70
#define PRIMARY_MUX_ADDR		MUX_BASE_ADDR

#define NUM_SUPER_NODES		1

#define NUM_IMU_NODES		3
#define NUM_MOTOR_NODES		4
#define NUM_NODES 			(NUM_IMU_NODES + NUM_MOTOR_NODES)

//motors list
Motor motors[NUM_MOTOR_NODES] = {
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0},
	{MOTOR_STATE_OFF, 0, 0}
};

//nodes init
SuitNet_Node nodes[NUM_NODES] = {
	{SUITNET_N_IMU, 0, 4, NULL},
	{SUITNET_N_IMU, 0, 5, NULL},
	{SUITNET_N_IMU, 0, 7, NULL},
	{SUITNET_N_MOTOR, 0, 0, motors},
	{SUITNET_N_MOTOR, 0, 1, motors + 1},
	{SUITNET_N_MOTOR, 0, 2, motors + 2},
	{SUITNET_N_MOTOR, 0, 3, motors + 3}
};

//supernode init
SuitNet_SuperNode superNodes[NUM_SUPER_NODES] = {
	{-1, SUITNET_SNODE_STAT_UNKNOWN, nodes, 3, MUX_BASE_ADDR | 0x07} //0
};

#endif
