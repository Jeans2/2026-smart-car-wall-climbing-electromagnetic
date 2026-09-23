#ifndef __XUNJI_H
#define __XUNJI_H

#include "bsp_system.h"

#define huan_zhi 1
#define huan_ru 2
#define huan_chu 3

extern int8 element;
extern int8 ring_flag_task;
extern int16 speed_straight;
extern int16 speed_ringR;
extern uint16 qqb_low_speed_delay;
extern int16 qqb_low_speed_target;
extern uint8 ring_plan_laps;
extern uint8 ring_plan_seq_len;
extern uint8 ring_plan_seq_pos;
extern uint32 ring_plan_seq_code;
extern uint8 ring_plan_stop_action;
extern volatile uint8 start_ramp_flag;

void xunji_state_reset(void);
void xunji(void);

#endif
