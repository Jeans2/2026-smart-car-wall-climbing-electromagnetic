/*********************************************************************************************************************
 * STC32G 开源库中断服务文件
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本项目在此文件中集中处理 DMA 串口、外部中断和定时器中断。
 * 开发环境: MDK FOR C251
 * 适用平台: STC32G
 ********************************************************************************************************************/
#include "zf_common_headfile.h"
static unsigned char timer_4ms_count = 0;
static unsigned char timer_10ms_count = 0;
void DMA_UART1_IRQHandler(void) interrupt 4
{
    static vuint8 dwon_count = 0;

    if (DMA_UR1R_STA & 0x01)
    {
        DMA_UR1R_STA &= ~0x01;
        uart_rx_start_buff(UART_1);

        // 串口停车命令
        if (uart_rx_buff[UART_1][0] == 'S' || uart_rx_buff[UART_1][0] == 's')
        {
            start_ramp_flag = 0;
        }

        // 程序自动下载
        if (uart_rx_buff[UART_1][0] == 0x7F)
        {
            if (dwon_count++ > 20)
            {
                IAP_CONTR = 0x60;
            }
        }
        else
        {
            dwon_count = 0;
        }

        if (uart1_irq_handler != NULL)
        {
            uart1_irq_handler(uart_rx_buff[UART_1][0]);
        }
    }

    if (DMA_UR1R_STA & 0x02)
    {
        DMA_UR1R_STA &= ~0x02;
        uart_rx_start_buff(UART_1);
    }
}

void DMA_UART2_IRQHandler(void) interrupt 8
{
    if (DMA_UR2R_STA & 0x01)
    {
        DMA_UR2R_STA &= ~0x01;
        uart_rx_start_buff(UART_2);
        if (uart2_irq_handler != NULL)
            uart2_irq_handler(uart_rx_buff[UART_2][0]);
    }
    if (DMA_UR2R_STA & 0x02)
    {
        DMA_UR2R_STA &= ~0x02;
        uart_rx_start_buff(UART_2);
    }
}

void DMA_UART3_IRQHandler(void) interrupt 17
{
    if (DMA_UR3R_STA & 0x01)
    {
        DMA_UR3R_STA &= ~0x01;
        uart_rx_start_buff(UART_3);
        if (uart3_irq_handler != NULL)
            uart3_irq_handler(uart_rx_buff[UART_3][0]);
    }
    if (DMA_UR3R_STA & 0x02)
    {
        DMA_UR3R_STA &= ~0x02;
        uart_rx_start_buff(UART_3);
    }
}

void DMA_UART4_IRQHandler(void) interrupt 18
{
    if (DMA_UR4R_STA & 0x01)
    {
        DMA_UR4R_STA &= ~0x01;
        uart_rx_start_buff(UART_4);

       // 无线串口停车命令
        if (uart_rx_buff[UART_4][0] == 'S' || uart_rx_buff[UART_4][0] == 's')
        {
            start_ramp_flag = 0;
        }

        if (uart4_irq_handler != NULL)
            uart4_irq_handler(uart_rx_buff[UART_4][0]);
    }
    if (DMA_UR4R_STA & 0x02)
    {
        DMA_UR4R_STA &= ~0x02;
        uart_rx_start_buff(UART_4);
    }
}

void INT0_IRQHandler() interrupt 0
{
    INT0_CLEAR_FLAG;
    if (int0_irq_handler != NULL)
        int0_irq_handler();
}

void TM0_IRQHandler() interrupt 1
{
    TIM0_CLEAR_FLAG;
    if (tim0_irq_handler != NULL)
        tim0_irq_handler();
}




void TM1_IRQHandler() interrupt 3
{
    TIM1_CLEAR_FLAG;

    if (tim1_irq_handler != NULL)
    {
        tim1_irq_handler();
        angle_get();

        timer_4ms_count++;
        timer_10ms_count++;

        if (timer_4ms_count >= 2)  // 2ms * 2 = 4ms
        {
            timer_4ms_count = 0;
            caiyang();
            adc_differ();

            if (element == 1 && ring_flag_task == huan_ru)
            {
                deviation = deviation -50	;
            }
            if (element == 2 && ring_flag_task == huan_ru)
            {
                deviation = deviation +50;
            }
            if (element == 1 && ring_flag_task == huan_chu)
            {
                deviation = deviation +20 ;
            }
            if (element == 2 && ring_flag_task == huan_chu)
            {
                deviation = deviation +20 ;
            }

            direction_loop(deviation);
        }

        if (timer_10ms_count >= 5)  // 2ms * 5 = 10ms
        {
            timer_10ms_count = 0;
            speed_debug_send_flag = 1;
        }

//				speed_loop_LR(speed_straight, speed_straight);
//        set_pwm_motor_R(out_R);
//        set_pwm_motor_L(0);
					
        {
            static uint16 ramp_cnt = 0;
            static uint8 ramp_output_state = 0;
            switch (start_ramp_flag)
            {
                case 1:
                    if (ramp_output_state == 0)
                    {
                        ramp_output_state = 1;
                        fuya_start_request = 1;
                    }
                    if (ramp_cnt == 0) ips114_clear_request = 1;  // 发车时关屏
                    if (ramp_cnt < 500)  // 2ms * 500 = 1000ms
                        ramp_cnt++;
                    else
                        xunji();
                    break;

                case 0:
                    ramp_cnt = 0;
                    if (ramp_output_state != 0)
                    {
                        ramp_output_state = 0;
                        motor_stop_request = 1;
                    }
                    break;
										
            }
        }
    }
}
void TM2_IRQHandler() interrupt 12
{
    TIM2_CLEAR_FLAG;
    if (tim2_irq_handler != NULL)
        tim2_irq_handler();
}

void TM3_IRQHandler() interrupt 19
{
    TIM3_CLEAR_FLAG;
    if (tim3_irq_handler != NULL)
        tim3_irq_handler();
}

void TM4_IRQHandler() interrupt 20
{
    TIM4_CLEAR_FLAG;
    if (tim4_irq_handler != NULL)
        tim4_irq_handler();
}
