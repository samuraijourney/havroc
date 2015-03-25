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
#define NUM_MOTOR_NODES		8
#define NUM_NODES 			(NUM_IMU_NODES + NUM_MOTOR_NODES)

//motors list
Motor motors[NUM_MOTOR_NODES] = {
	{ 0, 1},
	{ 0, 1},
	{ 0, 1},
	{ 0, 1},
	{ 0, 1},
	{ 0, 1},
	{ 0, 1},
	{ 0, 1}
};

//nodes init
SuitNet_Node nodes[NUM_NODES] = {
	{SUITNET_N_IMU, 0, 0, NULL},
	{SUITNET_N_IMU, 0, 1, NULL},
	{SUITNET_N_IMU, 0, 0, NULL},
	{SUITNET_N_MOTOR, 0, 0, motors + 0},
	{SUITNET_N_MOTOR, 0, 1, motors + 1},
	{SUITNET_N_MOTOR, 0, 2, motors + 2},
	{SUITNET_N_MOTOR, 0, 3, motors + 3},
	{SUITNET_N_MOTOR, 0, 4, motors + 4},
	{SUITNET_N_MOTOR, 0, 5, motors + 5},
	{SUITNET_N_MOTOR, 0, 6, motors + 6},
	{SUITNET_N_MOTOR, 0, 7, motors + 7}
};

//supernode init
SuitNet_SuperNode superNodes[NUM_SUPER_NODES] = {
	{-1, SUITNET_SNODE_STAT_UNKNOWN, nodes, 11, MUX_BASE_ADDR | 0x07} //0
};

#endif
