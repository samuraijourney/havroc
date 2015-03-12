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
	TRACKING_CMD = 1,
	SYSTEM_CMD,
	MOTOR_CMD,
	CMD_MAX
};

enum Tracking_Messages
{
	STATE_CMD = 10,
	DATA_CMD,
	TRACKING_MAX
};

enum Motor_Messages
{
	DATA_CMD = 20,
	MOTOR_MAX
};

enum System_Messages
{
	KILL_CMD = 30,
	MOTOR_MAX
};

#define ERROR_CMD		   0
#define HEADER_BYTES_CNT 5

#endif /* COMMAND_H_ */
