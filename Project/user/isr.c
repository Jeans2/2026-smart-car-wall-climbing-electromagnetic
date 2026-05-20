/*********************************************************************************************************************
 * STC32G Opensourec Library 即（STC32G 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是STC 开源库的一部分
 *
 * STC32G 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          MDK FOR C251
 * 适用平台          STC32G
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者           备注
 * 2024-08-01        大W            first version
 ********************************************************************************************************************/

#include "zf_common_headfile.h"
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
        encoder_update();
        angle_get();

        // 环岛入环直行距离累积
        if (element == 1 && ringR_flag_task == 1)
            ring_entry_dist += speed_avl;

        timer_10ms_count++;

        if (timer_10ms_count >= 5)  // 2ms * 5 = 10ms
        {
            timer_10ms_count = 0;
            caiyang();
            adc_differ();

            if (element == 1 && ringR_flag_task == 1 && ring_entry_dist >= 1000)
                deviation = deviation + 35;
            if (element == 1 && ringR_flag_task == 4)
                deviation = deviation - 40;

            direction_loop(deviation);
        }

        {
            static uint16 ramp_cnt = 0;

            switch (start_ramp_flag)
            {
                case 1:
                    fuya_set_duty(7000);
                    if (ramp_cnt == 0) ips114_clear(0x0000);  // 发车时关屏
                    if (ramp_cnt < 250)  // 2ms * 250 = 500ms
                        ramp_cnt++;
                    else
                        xunji();
                    break;

                case 0:
                    ramp_cnt = 0;
                    set_pwm_motor_R(0);
                    set_pwm_motor_L(0);
                    fuya_set_duty(0);
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
