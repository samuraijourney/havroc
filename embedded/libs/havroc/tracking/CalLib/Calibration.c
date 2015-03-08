#include "havroc/tracking/CalLib/Calibration.h"
#include "havroc/id.h"

void init_calib ()
{
	magMin[R_SHOULDER_IMU_ID][0] = R_S_MIN_X;
	magMin[R_SHOULDER_IMU_ID][1] = R_S_MIN_Y;
	magMin[R_SHOULDER_IMU_ID][2] = R_S_MIN_Z;

	magMax[R_SHOULDER_IMU_ID][0] = R_S_MAX_X;
	magMax[R_SHOULDER_IMU_ID][1] = R_S_MAX_Y;
	magMax[R_SHOULDER_IMU_ID][2] = R_S_MAX_Z;

	magMin[L_SHOULDER_IMU_ID][0] = L_S_MIN_X;
	magMin[L_SHOULDER_IMU_ID][1] = L_S_MIN_Y;
	magMin[L_SHOULDER_IMU_ID][2] = L_S_MIN_Z;

	magMax[L_SHOULDER_IMU_ID][0] = L_S_MAX_X;
	magMax[L_SHOULDER_IMU_ID][1] = L_S_MAX_Y;
	magMax[L_SHOULDER_IMU_ID][2] = L_S_MAX_Z;

	magMin[R_ELBOW_IMU_ID][0] = L_E_MIN_X;
	magMin[R_ELBOW_IMU_ID][1] = R_E_MIN_Y;
	magMin[R_ELBOW_IMU_ID][2] = R_E_MIN_Z;

	magMax[R_ELBOW_IMU_ID][0] = R_E_MAX_X;
	magMax[R_ELBOW_IMU_ID][1] = R_E_MAX_Y;
	magMax[R_ELBOW_IMU_ID][2] = R_E_MAX_Z;

	magMin[L_ELBOW_IMU_ID][0] = L_E_MIN_X;
	magMin[L_ELBOW_IMU_ID][1] = L_E_MIN_Y;
	magMin[L_ELBOW_IMU_ID][2] = L_E_MIN_Z;

	magMax[L_ELBOW_IMU_ID][0] = L_E_MAX_X;
	magMax[L_ELBOW_IMU_ID][1] = L_E_MAX_Y;
	magMax[L_ELBOW_IMU_ID][2] = L_E_MAX_Z;

	magMin[L_WRIST_IMU_ID][0] = L_W_MIN_X;
	magMin[L_WRIST_IMU_ID][1] = L_W_MIN_Y;
	magMin[L_WRIST_IMU_ID][2] = L_W_MIN_Z;

	magMax[L_WRIST_IMU_ID][0] = L_W_MAX_X;
	magMax[L_WRIST_IMU_ID][1] = L_W_MAX_Y;
	magMax[L_WRIST_IMU_ID][2] = L_W_MAX_Z;

	magMin[R_WRIST_IMU_ID][0] = R_W_MIN_X;
	magMin[R_WRIST_IMU_ID][1] = R_W_MIN_Y;
	magMin[R_WRIST_IMU_ID][2] = R_W_MIN_Z;

	magMax[R_WRIST_IMU_ID][0] = R_W_MAX_X;
	magMax[R_WRIST_IMU_ID][1] = R_W_MAX_Y;
	magMax[R_WRIST_IMU_ID][2] = R_W_MAX_Z;
}
