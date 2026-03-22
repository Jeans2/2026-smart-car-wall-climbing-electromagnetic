#include "key.h"

// 开关状态变量 (默认上拉为 1)
uint8 key1_status = 1;  uint8 key2_status = 1;
uint8 key3_status = 1;  uint8 key4_status = 1;
uint8 key1_last_status = 1; uint8 key2_last_status = 1;
uint8 key3_last_status = 1; uint8 key4_last_status = 1;

void key_init(void)
{
	system_delay_init();
	gpio_init(IO_P70,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P71,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P72,GPIO,1,GPIO_NO_PULL);
	gpio_init(IO_P73,GPIO,1,GPIO_NO_PULL);
	
	
}

//void Key_PID_Adjust(void)
//{
//    // 使用 static 修饰，记忆上一次的按键状态，且不污染全局变量
//    static uint8 key1_last = 1, key2_last = 1, key3_last = 1, key4_last = 1;
//    uint8 key1_now, key2_now, key3_now, key4_now;

//    // 1. 读取当前 4 个按键的电平状态

//    key1_now = gpio_get_level(KEY1_PIN);
//    key2_now = gpio_get_level(KEY2_PIN);
//    key3_now = gpio_get_level(KEY3_PIN);
//    key4_now = gpio_get_level(KEY4_PIN);



//    // 2. 按键 1：增加 Kp (按下瞬间触发)
//    if(key1_now == 0 && key1_last == 1)    
//    {
//        system_delay_ms(15); // 软件消抖避开机械毛刺
//        if(gpio_get_level(KEY1_PIN) == 0) // 再次确认按键状态
//        {
//            pid_motor_run.Kp += 0.1f;
//            if(pid_motor_run.Kp > 5.0f) pid_motor_run.Kp = 5.0f; // 极限保护
//        }
//    }
//    
//    // 3. 按键 2：减小 Kp
//    if(key2_now == 0 && key2_last == 1)    
//    {
//        system_delay_ms(15);
//        if(gpio_get_level(KEY2_PIN) == 0)
//        {
//            pid_motor_run.Kp -= 0.1f;
//            if(pid_motor_run.Kp < 0.0f) pid_motor_run.Kp = 0.0f; // 防止调成负数
//        }
//    }
//    
//    // 4. 按键 3：增加 Kd
//    if(key3_now == 0 && key3_last == 1)    
//    {
//        system_delay_ms(15);
//        if(gpio_get_level(KEY3_PIN) == 0)
//        {
//            pid_motor_run.Kd += 0.1f;   // D 参数非常敏感，步长设为 0.01
//            if(pid_motor_run.Kd > 2.0f) pid_motor_run.Kd = 2.0f; 
//        }
//    }
//    
//    // 5. 按键 4：减小 Kd
//    if(key4_now == 0 && key4_last == 1)    
//    {
//        system_delay_ms(15);
//        if(gpio_get_level(KEY4_PIN) == 0)
//        {
//            pid_motor_run.Kd -= 0.1f;
//            if(pid_motor_run.Kd < 0.0f) pid_motor_run.Kd = 0.0f; 
//        }
//    }

//    // 6. 更新历史状态，为下一次扫描做准备
//    key1_last = key1_now;
//    key2_last = key2_now;
//    key3_last = key3_now;
//    key4_last = key4_now;
//}