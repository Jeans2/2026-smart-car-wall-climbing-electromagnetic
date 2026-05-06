#include "pid.h"

struct PID pid_motor_run = {3.24f, 0, 2.3f, 0, 0, 0};
struct PID pid_loop_speed = {24.0f, 0.2f, 0, 0, 0, 0};

int16 err_speed = 0;
int16 speed_target = 0;
float out_L = 0, out_R = 0;
int16 err_speed_L_last = 0, err_speed_L = 0;
int16 err_speed_R_last = 0, err_speed_R = 0;
static float speed_output_L = 0;
static float speed_output_R = 0;
float correct_L = 0;
float direction_err1[4] = {0};

void speed_loop_LR(int16 speed_L_t, int16 speed_R_t)
{
	err_speed_L_last = err_speed_L;
	err_speed_L = speed_L_t - speed_L;

	speed_output_L = pid_loop_speed.Kp * (err_speed_L - err_speed_L_last)
	               + pid_loop_speed.Ki * err_speed_L;

	if (speed_output_L > 2000)  speed_output_L = 2000;
	if (speed_output_L < -2000) speed_output_L = -2000;

	out_L += speed_output_L;

	err_speed_R_last = err_speed_R;
	err_speed_R = speed_R_t - speed_R;

	speed_output_R = pid_loop_speed.Kp * (err_speed_R - err_speed_R_last)
	               + pid_loop_speed.Ki * err_speed_R;

	if (speed_output_R > 2000)  speed_output_R = 2000;
	if (speed_output_R < -2000) speed_output_R = -2000;

	out_R += speed_output_R;

	if (out_L > 3500)  out_L = 3500;
	if (out_L < -3500) out_L = -3500;
	if (out_R > 3500)  out_R = 3500;
	if (out_R < -3500) out_R = -3500;
}

float direction_loop(float err_position)
{
	static float D_err = 0;

	direction_err1[2] = direction_err1[1];
	direction_err1[1] = direction_err1[0];
	direction_err1[0] = err_position;

	D_err = direction_err1[0] - direction_err1[1];

	correct_L = pid_motor_run.Kp * direction_err1[0]
	          + pid_motor_run.Kd * (direction_err1[0] - direction_err1[1]);

	if (correct_L > 10000)  correct_L = 10000;
	if (correct_L < -10000) correct_L = -10000;

	return correct_L;
}
