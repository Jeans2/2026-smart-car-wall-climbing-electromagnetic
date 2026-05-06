#include "key.h"

// ����״̬���� (Ĭ������Ϊ 1)
uint8 key1_status = 1;  uint8 key2_status = 1;
uint8 key3_status = 1;  uint8 key4_status = 1;  uint8 key5_status = 1;
uint8 key1_last_status = 1; uint8 key2_last_status = 1;
uint8 key3_last_status = 1; uint8 key4_last_status = 1;
uint8 key5_last_status = 1;

void key_init(void)
{
	system_delay_init();
	gpio_init(IO_P70,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P71,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P72,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P73,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P75,GPIO,1,GPIO_NO_PULL);
	
	
}

