#ifndef __IMU660RB_H
#define __IMU660RB_H
#include "bsp_system.h"


//extern float angle_x;
extern float angle_z;
//extern float avl_gyro_z;
//extern float gyro_z_filtered;
//extern float gyro_x_filtered;
//extern float acc_z_filtered;
//extern float acc_x_filtered;
//extern float acc_y_filtered;
extern float pitch;


void angle_get(void);
void angle_clear(void);
void gyro_get_x(void);
void gyro_get_z(void);
void gyro_init_calibration(void);
void get_acc_z(void);
void get_acc_x(void);
void get_acc_y(void);

#endif

////extern float gyro_z[2], next_gyro_z;			//(被测)Z轴角速度
//extern float angle_ringR;
//extern float null_drift_z;
//extern float avl_gyro_z;  //测量-零漂
