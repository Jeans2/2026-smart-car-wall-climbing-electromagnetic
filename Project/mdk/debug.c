#include "debug.h"

static char debug_dat[48];

volatile uint8 xunji_debug_event = 0;
volatile uint8 xunji_debug_ring_index = 0;
volatile float xunji_debug_L = 0;
volatile float xunji_debug_LM = 0;
volatile float xunji_debug_RM = 0;
volatile float xunji_debug_R = 0;

void debug_uart_send(void)
{
    uint8 event = 0;

    if (xunji_debug_event == 0)
        return;

    EA = 0;
    event = xunji_debug_event;
    xunji_debug_event = 0;
    EA = 1;

    if (event == 8)
    {
        sprintf(debug_dat, "ring:%d\r\n", xunji_debug_ring_index);
        wireless_uart_send_string(debug_dat);
        return;
    }

    if (event == 1)
        sprintf(debug_dat, "huan L in\r\n");
    else if (event == 2)
        sprintf(debug_dat, "huan end\r\n");

    else if (event == 4)
        sprintf(debug_dat, "qqb end\r\n");

    else if (event == 6)
        sprintf(debug_dat, "huan R in\r\n");

    else if (event == 7)
        sprintf(debug_dat, "unlock\r\n");

    else
        return;

    wireless_uart_send_string(debug_dat);

    if (event == 1 || event == 6)
    {
        sprintf(debug_dat, "ring:%d\r\n", xunji_debug_ring_index);
        wireless_uart_send_string(debug_dat);

        sprintf(debug_dat, "L:%d LM:%d RM:%d R:%d\r\n",
                (int16)xunji_debug_L,
                (int16)xunji_debug_LM,
                (int16)xunji_debug_RM,
                (int16)xunji_debug_R);
        wireless_uart_send_string(debug_dat);
    }
}
