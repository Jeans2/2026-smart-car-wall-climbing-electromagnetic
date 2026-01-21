#include "motor.h"



void motor_init(void)
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                            // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_L, 17000, 0);                                                  // PWM 通道初始化频率 17KHz 占空比初始为 0

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                            // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_R, 17000, 0);    
	
}

void set_pwm_motor_R(int16 duty)
{
	  if(duty>9900){duty=9900;}
	  if(duty<-9900){duty=-9900;}
		
		if(duty>0)
		{
		gpio_set_level(DIR_L, GPIO_HIGH);
		pwm_set_duty(PWM_L,duty);
		}
		else
		{
		gpio_set_level(DIR_L, GPIO_LOW);
		pwm_set_duty(PWM_L,-duty);
		}
}


void set_pwm_motor_L(int16 duty)
{
	  if(duty>9900){duty=9900;}
	  if(duty<-9900){duty=-9900;}
		
		if(duty>0)
		{
		gpio_set_level(DIR_R, GPIO_HIGH);
		pwm_set_duty(PWM_R,duty);
		}
		else
		{
		gpio_set_level(DIR_R, GPIO_LOW);
		pwm_set_duty(PWM_R,-duty);
		}
}

