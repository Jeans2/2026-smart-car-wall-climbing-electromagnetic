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

/*********PID*********/

extern struct PID pid_motor_straight;
extern struct PID pid_motor_turn;

extern struct PID pid_loop_speed_start;	
extern struct PID pid_loop_speed_run;					
extern int16 speed_target;
extern int16 speed_straight,speed_turn,speed_ringR;

extern struct PID pid_loop_speed;



//循迹
extern struct PID pid_motor_run;
//extern struct PID pid_motor_ringR;
//extern struct PID pid_loop_angle;
//extern struct PID pid_loop_gyro;

//extern float limit_gyro  ;
//extern int16 speed_turn ;

//extern float angle_start;
//extern struct PID pid_loop_angle_start;				//角度环
//extern struct PID pid_loop_angle_ring;				//角度环

/*************变量*************/
//速度环
extern int16 err_speed;			//误差
extern float out_L,out_R;			//输出


//方向环
extern float expect_gyro;			//误差
extern float correct_L;				//输出


//角度环
//extern float expect_gyro_angles;
//extern float out_angle;

//角速度环
extern int16 out_gyro;




/*************函数*************/

//void speed_loop(void);
//void speed_loop_LR(void);



//float out();
//void speed_loop(void);//速度环
void speed_loop_LR(int16 speed_L_t,int16 speed_R_t);
float gyro_loop(float expect_gyro, float avl_gyro_z);
float direction_loop(float err_position);
//void direction_return(float err_position);//方向环
#endif

