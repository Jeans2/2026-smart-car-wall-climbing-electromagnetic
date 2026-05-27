#include "debug.h"

static char debug_dat[64];

volatile uint8 xunji_debug_event = 0;
volatile float xunji_debug_L = 0;
volatile float xunji_debug_LM = 0;
volatile float xunji_debug_RM = 0;
volatile float xunji_debug_R = 0;
volatile float xunji_debug_roll = 0;
volatile uint16 xunji_debug_distance = 0;

void debug_uart_send(void)
{
    uint8 event = 0;
    float data_L = 0;
    float data_LM = 0;
    float data_RM = 0;
    float data_R = 0;
    float data_roll = 0;
    uint16 distance = 0;

    if (xunji_debug_event == 0)
        return;

    EA = 0;
    event = xunji_debug_event;
    data_L = xunji_debug_L;
    data_LM = xunji_debug_LM;
    data_RM = xunji_debug_RM;
    data_R = xunji_debug_R;
    data_roll = xunji_debug_roll;
    distance = xunji_debug_distance;
    xunji_debug_event = 0;
    EA = 1;

    if (event == 1)
        sprintf(debug_dat, "QQB ok,LR:%f\r\n", data_L);
    else if (event == 2)
        sprintf(debug_dat, "L:%f,LM:%f,RM:%f,R:%f,distance:%u,roll:%f\r\n", data_L, data_LM, data_RM, data_R, distance, data_roll);
    else if (event == 3)
        sprintf(debug_dat, "Tong,LR:%f\r\n", data_L);
    else if (event == 4)
        sprintf(debug_dat, "Qiang,LR:%f\r\n", data_L);
    else if (event == 5)
        sprintf(debug_dat, "QQB unlock,roll:%f\r\n", data_roll);
    else
        return;
		
    wireless_uart_send_string(debug_dat);
}
