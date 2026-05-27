#ifndef __XUNJI_H
#define __XUNJI_H

#include "bsp_system.h"

extern int8 element;
extern int8 ringR_flag_task;
extern int16 speed_straight;
extern int16 speed_ringR;
extern uint8 start_ramp_flag;
extern float ring_entry_dist;

void xunji(void);

#endif
