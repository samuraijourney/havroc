#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

//for reference
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

#define R_S_MAX_X 65.45
#define R_S_MAX_Y 44.18
#define R_S_MAX_Z 14.68
#define R_S_MIN_X -38.72
#define R_S_MIN_Y -58.07
#define R_S_MIN_Z -92.88

const int calib_valid[6] = {1, 0, 0, 0, 0, 0};
const float compassCalScale[6][3] =
{
    {1.03,1.05,1.00},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

const float compassCalOffset[6][3] =
{
    {13.37,-6.95,-39.10},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

#endif // _CALIBRATION_H_
