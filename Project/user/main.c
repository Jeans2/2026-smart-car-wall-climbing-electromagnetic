#include "zf_common_headfile.h"
#include "main.h"

#define PIT_CH      (TIM1_PIT)
char buf[128];

volatile uint8 ips114_clear_request = 0;
volatile uint8 fuya_start_request = 0;
volatile uint8 motor_stop_request = 0;
volatile uint8 speed_debug_send_flag = 0;
char dat[64];

void main()
{
    clock_init(SYSTEM_CLOCK_30M);
    debug_init();
    System_Init();
    iap_init();
    Load_Params_From_EEPROM();
    pit_ms_init(PIT_CH, 2);
    interrupt_set_priority(TIMER1_IRQn, 3);
    tim1_irq_handler = encoder_update;

    while (1)
    {

        if (speed_debug_send_flag)
        {
            speed_debug_send_flag = 0;
            //sprintf(buf, "%d,%d,%d,%d,%d,%d\n",speed_target, speed_L, speed_R, speed_avl,(int16)deviation, (int16)correct_L);
            //sprintf(buf, "%d\n",(int16)deviation);
            //sprintf(buf, "%f,%f,%f,%f\n", L, LM, R, RM);
            //sprintf(buf, "%f,%f\n", L+R,LM+RM);
            //sprintf(buf, "%f,%f\n" ,L+R+LM+RM,imu660rc_roll);
            //sprintf(buf, "%f,%f\n" ,L+R,imu660rc_roll);
            wireless_uart_send_string(buf);
        }

        if (ips114_clear_request)
        {
            ips114_clear_request = 0;
            ips114_clear(0x0000);
        }

        if (motor_stop_request)
        {
            motor_stop_request = 0;
            set_pwm_motor_R(0);
            set_pwm_motor_L(0);
            fuya_set_duty(0);
        }

        if (fuya_start_request)
        {
            fuya_start_request = 0;
            fuya_set_duty(7000);
        }

        debug_uart_send();
        if (start_ramp_flag == 0)
        {
            Key_Menu_Adjust();
            UI_Display_Update();
        }
    }
}
