/*
 * motor.h
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "stdint.h"
#include "havroc/communications/suit/suit_i2c.h"

/*
 * State of motor
 */
typedef enum _MotorState
{
	MOTOR_STATE_OFF = 0,
	MOTOR_STATE_ON
} MotorState;

/*
 * Motor error codes
 */
typedef enum _MotorErrorCode
{
	//On success
	MOTOR_E_SUCCESS = 0,
	MOTOR_E_FAIL
} MotorErrorCode;

/**
 * Motor object;
 */
typedef struct _Motor
{
	uint8_t intensity;
	uint8_t calibrateComplete;
} Motor;

/* 
 * Calibrate selected motor;
 * Returns 0 on success; else see error code
 */
MotorErrorCode motor_calibrate(Motor *motor);
/*
 * Run selected motor at specific intensity level ()
 */
MotorErrorCode motor_run(Motor *motor, uint8_t intensity);
/*
 * Stop selected motor
 */
MotorErrorCode motor_stop(Motor *motor);


#endif /* MOTOR_H_ */
