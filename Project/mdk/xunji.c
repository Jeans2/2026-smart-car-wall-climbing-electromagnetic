#include "xunji.h"

static void ringR_task(void);
static void ringR_execute(void);

int8 element = 0;
int8 ringR_flag_task = 0;
static int8 ringR_flag_execute = 1;
static uint8 ring_flag_ing = 0;
static float distance_ringR = 0;
int16 speed_straight = 230;
int16 speed_ringR = 215;
static uint8 ring_data_sent = 0;

void xunji(void)
{
	if (L + LM >100&&abs(LM-L)<10&&pitch<40 &&ring_flag_ing == 0)
	{
		element = 1;
		ringR_flag_task = 1;
		ring_flag_ing = 200;
		angle_clear();
	}           

	if (L + LM + RM + R < 30)
	{
		element = 9;
	}

	switch (element)
	{
	case 0:
		ring_data_sent = 0;
		speed_target = speed_straight;
		speed_loop_LR(speed_target + correct_L, speed_target - correct_L);
		set_pwm_motor_R(out_R);
		set_pwm_motor_L(out_L);

		if (ring_flag_ing > 0)
		{
			ring_flag_ing--;
		}
		break;

	case 1:
		if (ring_data_sent == 0)
		{
//			char buf[64];
//			sprintf(buf, "L:%f,LM:%f,RM:%f,R:%f,pitch:%f\r\n",L,LM,RM,R,pitch);	
//			wireless_uart_send_string(buf);
			ring_data_sent = 1;
		}
		ringR_task();
		ringR_execute();
		break;

	case 9:
		start_ramp_flag = 0;
		break;

	default:
		break;
	}
}

static void ringR_task(void)
{
	switch (ringR_flag_task)
	{
	case 1:
		angle_get();
		if (angle_z < -260)
		{
			ringR_flag_task = 4;
			ringR_flag_execute = 4;
			distance_ringR = 0;
		}
		break;

	case 4:
		distance_ringR += speed_avl;
		if (distance_ringR > 30000)
		{
			distance_ringR = 0;
			ringR_flag_task = 0;
			ringR_flag_execute = 1;
			element = 0;
		}
		break;

	default:
		break;
	}
}

static void ringR_execute(void)
{
	float target_L = 0;
	float target_R = 0;

	switch (ringR_flag_execute)
	{
	case 1:
		speed_target = speed_ringR;
		target_L = speed_target + correct_L;
		target_R = speed_target - correct_L;
		break;

	case 4:
		speed_target = speed_ringR;
		target_L = speed_target + correct_L;
		target_R = speed_target - correct_L;
		break;

	default:
		break;
	}

	if (ringR_flag_execute != 0)
	{
		speed_loop_LR(target_L, target_R);
		set_pwm_motor_R(out_R);
		set_pwm_motor_L(out_L);
	if (L + LM + RM + R < 30)
		{
			start_ramp_flag = 0;
		}
	}
}
