/*********************************************************************************************************************
 * STC32G Opensourec Library ����STC32G ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
 * Copyright (c) 2022 SEEKFREE ��ɿƼ�?
 *
 * ���ļ���STC ��Դ���һ����?
 *
 * STC32G ��Դ�� ���������?
 * �����Ը���������������ᷢ����?GPL��GNU General Public License���� GNUͨ�ù�������֤��������
 * �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����?���޸���
 *
 * ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı��?
 * ����û�������������Ի��ʺ��ض���;�ı�֤
 * ����ϸ����μ�?GPL
 *
 * ��Ӧ�����յ�����Դ���ͬʱ�յ�һ��?GPL �ĸ���
 * ���û�У������<https://www.gnu.org/licenses/>
 *
 * ����ע����
 * ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
 * ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
 * ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
 * ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������?
 *
 * �ļ�����
 * ��˾����          �ɶ���ɿƼ����޹��?
 * �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
 * ��������          MDK FOR C251
 * ����ƽ̨          STC32G
 * ��������          https://seekfree.taobao.com/
 *
 * �޸ļ�¼
 * ����              ����           ��ע
 * 2024-08-01        ��W            first version
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

        // ����ͣ������
        if (uart_rx_buff[UART_1][0] == 'S' || uart_rx_buff[UART_1][0] == 's')
        {
            start_ramp_flag = 0;
        }

        // �����Զ�����
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

       //����ͣ��
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

      
        if (element == 1 && ringR_flag_task == 1)
            ring_entry_dist += speed_avl;

        timer_10ms_count++;

        if (timer_10ms_count >= 5)  // 2ms * 5 = 10ms
        {
            timer_10ms_count = 0;
            caiyang();
            adc_differ();

            if (element == 1 && ringR_flag_task == 1 && ring_entry_dist >= 1000)
                deviation = deviation -45;     //30
            if (element == 1 && ringR_flag_task == 4)
                deviation = deviation +30;     //40

            direction_loop(deviation);
        }
//				speed_loop_LR(speed_straight,speed_straight);
//        set_pwm_motor_R(out_R);
//        set_pwm_motor_L(out_L);
			
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
                    if (ramp_cnt == 0) ips114_clear_request = 1;  // ����ʱ����
                    if (ramp_cnt < 250)  // 2ms * 250 = 500ms
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
