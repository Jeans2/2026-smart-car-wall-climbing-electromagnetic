#ifndef __IMU660RB_H
#define __IMU660RB_H
#include "bsp_system.h"

extern float gyro_z[2], next_gyro_z;			//(被测)Z轴角速度
extern float angle_ringR;
extern float null_drift_z;
extern float avl_gyro_z;  //测量-零漂

void gyro_get(void);
void angle_get(void);
void angle_clear(void);
int8 null_drift_calculate();

#endif