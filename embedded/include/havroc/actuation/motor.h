/*
 * motor.h
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "stdint.h"
#include "havroc/communications/suit/suit_i2c.h"

/* 
 * Init selected motor's driver
 */
void motor_init();
/*
 * Run selected motor at specific intensity level ()
 */
void motor_run(uint8_t intensity);
/* Run the internal waveform
 * 1) PUNCH
 */
void motor_runWaveform();

#endif /* MOTOR_H_ */
