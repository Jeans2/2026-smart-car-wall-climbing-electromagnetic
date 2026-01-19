#ifndef __PID_H
#define __PID_H

#include "bsp_system.h"


/*********PID*********/
extern struct PID
{
	float Kp;
	float Ki;
	float Kd;
	float Kp_gyro;
	float Ki_gyro;
	float Kd_gyro;
};

/*************变量*************/
//速度环
extern int16 err_speed;			//误差
extern float out_L,out_R;			//输出


//方向环
//extern float expect_gyro;			//误差
//extern float correct_L;				//输出


//角度环
//extern float expect_gyro_angles;
//extern float out_angle;

//角速度环
//extern int16 out_gyro;

//void speed_loop(void);
//void speed_loop_LR(void);
#endif
