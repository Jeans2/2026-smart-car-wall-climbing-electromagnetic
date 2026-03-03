#include "xunji.h"

int16 turnspeed = 0 ;
int16 correct_speed_L,correct_speed_R;


void xunji()
{	
	
			turnspeed= (int16) out();
			correct_speed_L = speed_target - turnspeed;
			correct_speed_R = speed_target + turnspeed;
//	if(correct_speed_L>100){correct_speed_L=55;}
//	if(correct_speed_L<0){correct_speed_L=10;}
//	if(correct_speed_R>100){correct_speed_L=55;}
//	if(correct_speed_R<0){correct_speed_L=10;}
			speed_loop_LR(correct_speed_L,correct_speed_R);
			set_pwm_motor_R(out_R);
			set_pwm_motor_L(out_L);
}