#ifndef _TRACKING_MANAGER_H
#define _TRACKING_MANAGER_H

// Standard includes
#include <stdint.h>

uint8_t Setup_IMUs(uint8_t* imu_select, uint8_t count, uint8_t board_arm);
void Tracking_Update(uint8_t count);
void Tracking_Update_New(uint8_t count);
void Tracking_Publish();
void Tracking_Publish_Quaternion();
void Tracking_Publish_Error(uint8_t error_code);

#endif //_TRACKING_MANAGER_H
