#include "pid.h"

struct PID pid_motor_run = {2.7f, 0.0f, 25.0f}; 
struct PID pid_loop_speed_L = {140.0f, 0.0f, 8.0f};
struct PID pid_loop_speed_R = {140.0f, 0.0f, 8.0f};      

 
int16 err_speed = 0;
int16 speed_target = 0;
int16 left_target = 0, right_target = 0;
float out_L = 0, out_R = 0;
int16 err_speed_L_last = 0, err_speed_L_last2 = 0, err_speed_L = 0;
int16 err_speed_R_last = 0, err_speed_R_last2 = 0, err_speed_R = 0;
static float speed_output_L = 0;
static float speed_output_R = 0;
float correct_L = 0;

float gyro_turn_out = 0;
float direction_integral = 0;
float direction_err1[4] = {0};
static float direction_d_err = 0;


void pid_control_reset(void)
{
	err_speed = 0;
	speed_target = 0;
	left_target = 0;
	right_target = 0;
	out_L = 0;
	out_R = 0;
	err_speed_L_last = 0;
	err_speed_L_last2 = 0;
	err_speed_L = 0;
	err_speed_R_last = 0;
	err_speed_R_last2 = 0;
	err_speed_R = 0;
	speed_output_L = 0;
	speed_output_R = 0;
	correct_L = 0;
	gyro_turn_out = 0;
	direction_integral = 0;
	direction_err1[0] = 0;
	direction_err1[1] = 0;
	direction_err1[2] = 0;
	direction_err1[3] = 0;
	direction_d_err = 0;
}
void speed_loop_LR(int16 speed_L_t, int16 speed_R_t)
{
	left_target = speed_L_t;
	right_target = speed_R_t;
	err_speed_L_last = err_speed_L;
	err_speed_L = speed_L_t - speed_L;

	speed_output_L = pid_loop_speed_L.Kp * err_speed_L
	               + pid_loop_speed_L.Kd * (err_speed_L - err_speed_L_last);

	err_speed_R_last = err_speed_R;
	err_speed_R = speed_R_t - speed_R;

	speed_output_R = pid_loop_speed_L.Kp * err_speed_R
	               + pid_loop_speed_L.Kd * (err_speed_R - err_speed_R_last);

	out_L = speed_output_L;
	out_R = speed_output_R;

	if (out_L > 7000)   out_L = 7000;
	if (out_L < -7000)  out_L = -7000;
	if (out_R > 7000)   out_R = 7000;
	if (out_R < -7000)  out_R = -7000;
}








float direction_loop(float err_position)
{
	direction_err1[2] = direction_err1[1];
	direction_err1[1] = direction_err1[0];
	direction_err1[0] = err_position;

	direction_d_err = direction_err1[0] - direction_err1[1];
	direction_integral += direction_err1[0];

	if (direction_integral > 2000)  direction_integral = 2000;
	if (direction_integral < -2000) direction_integral = -2000;

	gyro_turn_out = gyro_z_turn * 0.02f;

	correct_L = pid_motor_run.Kp * direction_err1[0]
	          + pid_motor_run.Ki * direction_integral
	          + pid_motor_run.Kd * direction_d_err
	          + gyro_turn_out;

	if (correct_L > 200)  correct_L = 200;
	if (correct_L < -200) correct_L = -200;

	return correct_L;
}
