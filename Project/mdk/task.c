#include "task.h"







void System_Init(void)
{
     
     my_adc_init();
     encoder_init();       // 编码器初始化
      motor_init();       // 电机初始化
    
}