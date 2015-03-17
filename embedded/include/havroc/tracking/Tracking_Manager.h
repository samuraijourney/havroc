#ifndef _TRACKING_MANAGER_H
#define _TRACKING_MANAGER_H

void Setup_IMUs(int imu_index);
void Tracking_Update(int imu_index);
void Tracking_Publish();

#endif //_TRACKING_MANAGER_H
