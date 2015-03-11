#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#define L_W_MAX_X 0
#define L_W_MAX_Y 0
#define L_W_MAX_Z 0
#define L_W_MIN_X 0
#define L_W_MIN_Y 0
#define L_W_MIN_Z 0

#define L_E_MAX_X 0
#define L_E_MAX_Y 0
#define L_E_MAX_Z 0
#define L_E_MIN_X 0
#define L_E_MIN_Y 0
#define L_E_MIN_Z 0

#define L_S_MAX_X 0
#define L_S_MAX_Y 0
#define L_S_MAX_Z 0
#define L_S_MIN_X 0
#define L_S_MIN_Y 0
#define L_S_MIN_Z 0

#define R_W_MAX_X 0
#define R_W_MAX_Y 0
#define R_W_MAX_Z 0
#define R_W_MIN_X 0
#define R_W_MIN_Y 0
#define R_W_MIN_Z 0

#define R_E_MAX_X 0
#define R_E_MAX_Y 0
#define R_E_MAX_Z 0
#define R_E_MIN_X 0
#define R_E_MIN_Y 0
#define R_E_MIN_Z 0

#define R_S_MAX_X 0
#define R_S_MAX_Y 0
#define R_S_MAX_Z 0
#define R_S_MIN_X 0
#define R_S_MIN_Y 0
#define R_S_MIN_Z 0

const int calib_valid[6] = {0,0,0,0,0,0};

extern int magMin[6][3];
extern int magMax[6][3];

void init_calib ();

#endif // _CALIBRATION_H_
