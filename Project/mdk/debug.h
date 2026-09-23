#ifndef __DEBUG_H
#define __DEBUG_H

#include "bsp_system.h"

extern volatile uint8 xunji_debug_event;
extern volatile uint8 xunji_debug_ring_index;
extern volatile float xunji_debug_L;
extern volatile float xunji_debug_LM;
extern volatile float xunji_debug_RM;
extern volatile float xunji_debug_R;


void debug_uart_send(void);

#endif
