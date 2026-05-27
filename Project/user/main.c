/*********************************************************************************************************************
* STC32G Opensourec Library 即（STC32G 开源库）是一个基于官�?SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是STC 开源库的一部分
*
* STC32G 开源库 是免费软�?* 您可以根据自由软件基金会发布�?GPL（GNU General Public License，即 GNU通用公共许可证）的条�?* �?GPL 的第3版（�?GPL3.0）或（您选择的）任何后来的版本，重新发布�?或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参�?GPL
*
* 您应该在收到本开源库的同时收到一�?GPL 的副�?* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明�?* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版�?* 许可申明英文版在 libraries/doc 文件夹下�?GPL3_permission_statement.txt 文件�?* 许可证副本在 libraries 文件夹下 即该文件夹下�?LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明�?*
* 文件名称
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环�?         MDK FOR C251
* 适用平台          STC32G
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作�?          备注
* 2024-08-01        大W            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "main.h"

#define PIT_CH      (TIM1_PIT)


volatile uint8 tof_update_flag = 0;
volatile uint8 ips114_clear_request = 0;
volatile uint8 fuya_start_request = 0;
volatile uint8 motor_stop_request = 0;
char dat[64];
void main()
{
    clock_init(SYSTEM_CLOCK_30M);
    debug_init();
    System_Init();
    gpio_init(IO_P52, GPO, 1, GPO_PUSH_PULL);
    iap_init();
    Load_Params_From_EEPROM();
    pit_ms_init(PIT_CH, 2);
    interrupt_set_priority(TIMER1_IRQn, 3);
    tim1_irq_handler = encoder_update;

    while (1)
    {
			
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
