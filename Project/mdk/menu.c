#include "menu.h"

// 光标菜单变量
#define MENU_ITEMS 5             // 菜单总项数
uint8 cursor_index = 0;          // 当前光标位置 (0:Kp, 1:Kd, 2:A, 3:B, 4:C)
char disp_buf[32];               // 屏幕显示缓存区


// =========================================================================
// 函数名：Key_Menu_Adjust
// 功  能：光标切换与对应参数的修改 (已彻底清理宏定义)
// =========================================================================
void Key_Menu_Adjust(void)
{
    static uint8 key1_last = 1, key2_last = 1, key3_last = 1, key4_last = 1;
    uint8 key1_now, key2_now, key3_now, key4_now;

    // 1. 统一使用官方库函数读取引脚状态，干净利落
    key1_now = gpio_get_level(KEY1_PIN);
    key2_now = gpio_get_level(KEY2_PIN);
    key3_now = gpio_get_level(KEY3_PIN);
    key4_now = gpio_get_level(KEY4_PIN); // 预留读取 KEY4

    // ----------------- 功能 1：切换调参光标 (KEY1) -----------------
    if(key1_now == 0 && key1_last == 1)    
    {
        system_delay_ms(15);
        if(gpio_get_level(KEY1_PIN) == 0)
        {
            cursor_index++; 
            if(cursor_index >= MENU_ITEMS) cursor_index = 0; // 循环光标
        }
    }
    
    // ----------------- 功能 2：增加当前参数 (KEY2) -----------------
    if(key2_now == 0 && key2_last == 1)    
    {
        system_delay_ms(15);
        if(gpio_get_level(KEY2_PIN) == 0)
        {
            switch(cursor_index)
            {
                case 0: pid_motor_run.Kp += 0.1f;         break;
                case 1: pid_motor_run.Kd += 0.1f;         break; 
                case 2: adc_set_differ.A += 0.1f;         break; 
                case 3: adc_set_differ.B += 0.1f;         break;
                case 4: adc_set_differ.C += 0.1f;         break;
            }
        }
    }
    
    // ----------------- 功能 3：减小当前参数 (KEY3) -----------------
    if(key3_now == 0 && key3_last == 1)    
    {
        system_delay_ms(15);
        if(gpio_get_level(KEY3_PIN) == 0)
        {
            switch(cursor_index)
            {
                case 0: pid_motor_run.Kp -= 0.1f;         if(pid_motor_run.Kp < 0) pid_motor_run.Kp = 0; break;
                case 1: pid_motor_run.Kd -= 0.1f;         if(pid_motor_run.Kd < 0) pid_motor_run.Kd = 0; break;
                case 2: adc_set_differ.A -= 0.1f;         if(adc_set_differ.A < 0) adc_set_differ.A = 0; break;
                case 3: adc_set_differ.B -= 0.1f;         if(adc_set_differ.B < 0) adc_set_differ.B = 0; break;
                case 4: adc_set_differ.C -= 0.1f;         if(adc_set_differ.C < 0) adc_set_differ.C = 0; break;
            }
        }
    }

    // ----------------- 功能 4：预留位 (KEY4) -----------------
    if(key4_now == 0 && key4_last == 1)    
    {
        system_delay_ms(15);
        if(gpio_get_level(KEY4_PIN) == 0)
        {
            // 这里留空，一会儿可以把“一键保存到Flash”的功能写进来
        }
    }

    // 更新历史状态
    key1_last = key1_now;
    key2_last = key2_now;
    key3_last = key3_now;
    key4_last = key4_now;
}


// =========================================================================
// 函数名：UI_Display_Update
// 功  能：动态显示光标和参数列表
// =========================================================================
void UI_Display_Update(void)
{
    char prefix;

    prefix = (cursor_index == 0) ? '>' : ' '; 
    sprintf(disp_buf, "%c Turn Kp: %.2f  ", prefix, pid_motor_run.Kp);
    ips114_show_string(0, 0*16, disp_buf);
    
    prefix = (cursor_index == 1) ? '>' : ' ';
    sprintf(disp_buf, "%c Turn Kd: %.2f  ", prefix, pid_motor_run.Kd);
    ips114_show_string(0, 1*16, disp_buf);
    
    prefix = (cursor_index == 2) ? '>' : ' ';
    sprintf(disp_buf, "%c Form A : %.1f  ", prefix, adc_set_differ.A);
    ips114_show_string(0, 2*16, disp_buf);
    
    prefix = (cursor_index == 3) ? '>' : ' ';
    sprintf(disp_buf, "%c Form B : %.1f  ", prefix, adc_set_differ.B);
    ips114_show_string(0, 3*16, disp_buf);
    
    prefix = (cursor_index == 4) ? '>' : ' ';
    sprintf(disp_buf, "%c Form C : %.2f  ", prefix, adc_set_differ.C);
    ips114_show_string(0, 4*16, disp_buf);
}











