/*
 * command.h
 *
 *  Created on: Jan 27, 2015
 *      Author: Akram
 */

#ifndef COMMAND_H_
#define COMMAND_H_

enum Command_Modules
{
	BASE_MOD = 0,
	TRACKING_MOD,
	SYSTEM_MOD,
	MOTOR_MOD,
	MAX_MOD
};

enum Tracking_Messages
{
	TRACKING_STATE_CMD = 10,
	TRACKING_DATA_CMD,
	TRACKING_MAX
};

enum Tracking_Mode
{
	RIGHT_ARM = 1,
	LEFT_ARM
};

enum Motor_Messages
{
	MOTOR_DATA_CMD = 20,
	MOTOR_MAX
};

enum System_Messages
{
	SYSTEM_KILL_CMD = 30,
	SYSTEM_HEART_BEAT,
	SYSTEM_MAX
};

#define ERROR_CMD		   0
#define HEADER_BYTES_CNT 5

#endif /* COMMAND_H_ */
