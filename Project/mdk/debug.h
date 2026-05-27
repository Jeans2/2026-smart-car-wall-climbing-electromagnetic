#ifndef __DEBUG_H
#define __DEBUG_H

#include "bsp_system.h"

extern volatile uint8 xunji_debug_event;
extern volatile float xunji_debug_L;
extern volatile float xunji_debug_LM;
extern volatile float xunji_debug_RM;
extern volatile float xunji_debug_R;
extern volatile float xunji_debug_roll;
extern volatile uint16 xunji_debug_distance;

void debug_uart_send(void);

#endif
