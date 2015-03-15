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

#define L_S_MAX_X 63.06
#define L_S_MAX_Y 28.33
#define L_S_MAX_Z 61.84
#define L_S_MIN_X -19.31
#define L_S_MIN_Y -27.17
#define L_S_MIN_Z -21.71

#define R_W_MAX_X 66.04
#define R_W_MAX_Y 28.99
#define R_W_MAX_Z -8.16
#define R_W_MIN_X -20.19
#define R_W_MIN_Y -64.07
#define R_W_MIN_Z -107.25

#define R_E_MAX_X 67.97
#define R_E_MAX_Y 26.71
#define R_E_MAX_Z 28.47
#define R_E_MIN_X -21.46
#define R_E_MIN_Y -67.80
#define R_E_MIN_Z -66.33

#define R_S_MAX_X 65.45
#define R_S_MAX_Y 44.18
#define R_S_MAX_Z 14.68
#define R_S_MIN_X -38.72
#define R_S_MIN_Y -58.07
#define R_S_MIN_Z -92.88

const int calib_valid[6] = {1, 1, 1, 1, 0, 0};
const float compassCalScale[6][3] =
{
    {1.03,1.05,1.00},
    {1.06,1.00,1.00},
    {1.15,1.06,1.00},
    {1.01,1.51,1.00},
    {0,0,0},
    {0,0,0}
};

const float compassCalOffset[6][3] =
{
    {13.37,-6.95,-39.10},
    {23.25,-20.55,-18.93},
    {22.93,-17.54,-57.71},
    {21.88,0.58,20.06},
    {0,0,0},
    {0,0,0}
};


#endif // _CALIBRATION_H_
