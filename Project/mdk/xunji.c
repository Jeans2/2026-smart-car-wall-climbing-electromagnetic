#include "xunji.h"

int16 turnspeed = 10 ;
float zhuan;
int16 correct_speed_L,correct_speed_R;


void xunji()
{	
	
	turnspeed= (int16) out();
//	if(turnspeed > 50){	zhuanL = 1;	}	
//	else if(turnspeed < -50){zhuanR=1;} 
//	else if(turnspeed > -10 && turnspeed < 10){zhuanL =0;zhuanR =0;}
//	
//	if(zhuanL ==0 && zhuanR ==0)  //直行
//	{
//		while(1)
//		{
			correct_speed_L = speed_target - turnspeed;
			correct_speed_R = speed_target + turnspeed;
	if(correct_speed_L>50){correct_speed_L=15;}
	if(correct_speed_L<0){correct_speed_L=5;}
	if(correct_speed_R>50){correct_speed_L=15;}
	if(correct_speed_R<0){correct_speed_L=5;}
			speed_loop_LR(correct_speed_L,correct_speed_R);
			set_pwm_motor_R(out_R);
			set_pwm_motor_L(out_L);
//			if(turnspeed > 50){zhuanL = 1;break;}
//			else if(turnspeed < -50){zhuanR=1;break;}
//		}
//	}
//	if(zhuanL == 1)  //左转
//	{
//		while(1)
//		{
//			correct_speed_L = speed_target - turnspeed;
//			correct_speed_R = speed_target + turnspeed;
//			speed_loop_LR(correct_speed_L,correct_speed_R);
//			set_pwm_motor_R(out_R);
//			set_pwm_motor_L(out_L);
//			angle_get();
//			if(turnspeed > -10 && turnspeed < 10 && abs(angle) >90){zhuanL =0; angle_clear();break;}
//		}
//	}
//	if(zhuanR == 1)  //右转
//	{
//		while(1)
//		{	
//			correct_speed_L = speed_target + turnspeed;
//			correct_speed_R = speed_target - turnspeed;
//			speed_loop_LR(correct_speed_L,correct_speed_R);
//			set_pwm_motor_R(out_R);
//			set_pwm_motor_L(out_L);
//			if(turnspeed > -10 && turnspeed < 10 && abs(angle) >90){zhuanR =0;angle_clear();break;}
//		}
//	}
}