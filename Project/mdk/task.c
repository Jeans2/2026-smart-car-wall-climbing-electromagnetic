#include "task.h"







void System_Init(void)
{
     key_init();
    my_adc_init();
    encoder_init();       // 编码器初始化
    motor_init();       // 电机初始化
		ips114_init();
		imu660rb_init();
//		huan_init();
	  fuya_init();
	wireless_uart_init();
}

void caiyang(void)
{

	  siai_adc_all_sample();
	  adc_normalizing();
	
}