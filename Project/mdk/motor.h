#ifndef __MOTOR_H
#define __MOTOR_H

#include "bsp_system.h"

#define DIR_L               (IO_P64 )
#define PWM_L               (PWMA_CH4P_P66)
#define DIR_R               (IO_P60 )
#define PWM_R               (PWMA_CH2P_P62)


void motor_init(void);
void set_pwm_motor_L(int16 duty);
void set_pwm_motor_R(int16 duty);
#endif