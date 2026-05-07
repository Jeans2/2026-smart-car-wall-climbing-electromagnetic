#include "task.h"







void System_Init(void)
{
     key_init();
    my_adc_init();
    encoder_init();       
    motor_init();       
		ips114_init();
		imu660rc_init(IMU660RC_QUARTERNION_DISABLE);
	  fuya_init();
	wireless_uart_init();
		gyro_init_calibration();
}

void caiyang(void)
{

	  siai_adc_all_sample();
	  adc_normalizing();
	
}