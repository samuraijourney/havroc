/*
 * command.h
 *
 *  Created on: Jan 27, 2015
 *      Author: Akram
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#define CMD_BASE 0

enum
{
	TRACKING_CMD = 1,
	SYSTEM_CMD,
	MOTOR_CMD,
	ERROR_CMD,
	CMD_MAX
};


#endif /* COMMAND_H_ */
