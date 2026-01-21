#include "fuya.h"

void fuya_init()
{
	pwm_init(FUYA_CHL , 17000, 9000);
}

void fuya_set_duty(uint32 duty)
{
	pwm_set_duty(FUYA_CHL, duty);
}