#ifndef __FUYA_H_
#define __FUYA_H_

#include "zf_common_headfile.h"

#define FUYA_CHL   (PWMB_CH3_P33)

void fuya_init(void);
void fuya_set_duty(uint32 duty);
	
#endif