#ifndef __IMU660RB_H
#define __IMU660RB_H
#include "bsp_system.h"

extern float angle_z;
extern float pitch;

void angle_get(void);
void angle_clear(void);
void gyro_init_calibration(void);

#endif
