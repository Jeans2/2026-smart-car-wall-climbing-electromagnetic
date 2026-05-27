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
#define TOF_PIT_CH  (TIM4_PIT)

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
		interrupt_set_priority(TIMER1_IRQn, 3);  // TM1 最高优先级，不�?INT0 打断
		tim1_irq_handler = encoder_update;
		pit_ms_init(TOF_PIT_CH, 80);
		tim4_irq_handler = dl1a_get_distance;		
		
    while (1)
    {

        if (start_ramp_flag == 0)
        {
            Key_Menu_Adjust();
            UI_Display_Update();
        }
			//dl1a_get_distance();
					//sprintf(dat, "%d,%d,%d\r\n", speed_L,speed_R,230);
            //wireless_uart_send_string(dat);
        //printf("speed_L:%d,speed_R:%d,%d\r\n", speed_L,speed_R,230);
				//sprintf(dat, "%f,%f,%f,%f\r\n",L ,LM,RM,R);
				//sprintf(dat, "L:%f,LM:%f,RM:%f,R:%f,distance:%d\r\n", L, LM, RM, R, dl1a_distance_mm);
			//wireless_uart_send_string(dat);
    }
		
}
