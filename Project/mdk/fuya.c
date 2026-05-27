#include "fuya.h"

void fuya_init()
{
	pwm_init(FUYA_CHL ,17000,0 );
}

void fuya_set_duty(uint32 duty)
{
	pwm_set_duty_irq(FUYA_CHL, duty);
}