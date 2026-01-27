#ifndef __IMU660RB_H
#define __IMU660RB_H
#include "bsp_system.h"

extern float null_drift_z;	//¡„∆Ø
extern float angle;
extern float avl_gyro_z;

void gyro_get(void);
void angle_get(void);
void angle_clear(void);
void null_drift_calculate(void);

#endif

////extern float gyro_z[2], next_gyro_z;			//(±ª≤‚)Z÷·Ω«ÀŸ∂»
//extern float angle_ringR;
//extern float null_drift_z;
//extern float avl_gyro_z;  //≤‚¡ø-¡„∆Ø
