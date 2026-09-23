#ifndef __PID_H
#define __PID_H

#include "bsp_system.h"

struct PID
{
	float Kp;
	float Ki;
	float Kd;
};

extern struct PID pid_motor_run;
extern struct PID pid_loop_speed_L;
extern struct PID pid_loop_speed_R;

extern int16 err_speed;
extern int16 speed_target;
extern int16 left_target;
extern int16 right_target;
extern float out_L,out_R;
extern int16 err_speed_L_last;
extern int16 err_speed_L_last2;
extern int16 err_speed_L;
extern int16 err_speed_R_last;
extern int16 err_speed_R_last2;
extern int16 err_speed_R;
extern float direction_err1[4];
extern float direction_integral;
extern float correct_L;
extern float gyro_turn_k;
extern float gyro_turn_out;

void pid_control_reset(void);
void speed_loop_LR(int16 speed_L_t,int16 speed_R_t);
float direction_loop(float err_position);

#endif
