#ifndef H_MOTOR_ACTUATION
#define H_MOTOR_ACTUATION

#include <stdint.h>
#include <utils.h>

//Device Addresses
#define DRV2604L_ADDR 		0x5A
#define delay(ms) 			UtilsDelay((8000/5)*ms)


#ifdef __cplusplus 
extern "C"
{ 
#endif
	
	void erm_setup(int motor_index);

#ifdef __cplusplus 
} 
#endif

#endif
