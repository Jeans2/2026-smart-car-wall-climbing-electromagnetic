#include "menu.h"
#include "string.h"
// ===================【多级菜单状态定义】===================
#define MENU_MAIN       0  // 主菜单
#define MENU_TUNE_CATE  1  // 调参分类菜单
#define MENU_OBSERVE    2  // 数据观测界面
#define MENU_SPEED      3  // 速度环调节
#define MENU_TURN       4  // 转向环调节
#define MENU_FORM       5  // 差比和调节

uint8 menu_state = MENU_MAIN;    // 默认上电在主菜单
uint8 cursor_index = 0;          // 当前光标位置 
uint8 menu_max_items = 3;        // 当前页面的最大选项数 (动态变化)

char disp_buf[32];               // 屏幕显示缓存区
uint8 car_state = 0;             // 0:停车 1:发车
uint8 start_ramp_flag = 0;

// 定义参数存储的首地址（默认放在第0扇区最开头）
#define EEPROM_PARAM_ADDR   0x0000 
// 定义一个校验标志，用于判断芯片是不是第一次烧录程序
#define EEPROM_INIT_FLAG    0x5A


// =========================================================================
// 函数名：Save_Params_To_EEPROM
// 功  能：将所有调参数据打包并保存到 EEPROM
// =========================================================================
void Save_Params_To_EEPROM(void)
{
    uint8 save_buf[29]; // 1个标志位 + 7个float(共28字节) = 29字节
    
    save_buf[0] = EEPROM_INIT_FLAG; // 写入校验标志位
    
    // 将 float 参数转换为字节流存入数组
    memcpy(&save_buf[1],  &pid_loop_speed.Kp, 4);
    memcpy(&save_buf[5],  &pid_loop_speed.Ki, 4);
    memcpy(&save_buf[9],  &pid_motor_run.Kp,  4);
    memcpy(&save_buf[13], &pid_motor_run.Kd,  4);
    memcpy(&save_buf[17], &adc_set_differ.A,  4);
    memcpy(&save_buf[21], &adc_set_differ.B,  4);
    memcpy(&save_buf[25], &adc_set_differ.C,  4);

    // 调用逐飞库的“读-改-写”安全函数写入数据
    extern_iap_write_buff(EEPROM_PARAM_ADDR, save_buf, 29);
}

// =========================================================================
// 函数名：Load_Params_From_EEPROM
// 功  能：上电时从 EEPROM 读取数据。如果是空芯片则使用默认值并初始化。
// =========================================================================
void Load_Params_From_EEPROM(void)
{
    uint8 flag = 0;
    
    // 先只读第一个字节，看看是不是我们的校验标志
    flag = iap_read_byte(EEPROM_PARAM_ADDR);
    
    if(flag == EEPROM_INIT_FLAG)
    {
        // 如果是 0x5A，说明以前保存过数据，直接读取解包
        uint8 read_buf[28];
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_buf, 28);
        
        memcpy(&pid_loop_speed.Kp, &read_buf[0],  4);
        memcpy(&pid_loop_speed.Ki, &read_buf[4],  4);
        memcpy(&pid_motor_run.Kp,  &read_buf[8],  4);
        memcpy(&pid_motor_run.Kd,  &read_buf[12], 4);
        memcpy(&adc_set_differ.A,  &read_buf[16], 4);
        memcpy(&adc_set_differ.B,  &read_buf[20], 4);
        memcpy(&adc_set_differ.C,  &read_buf[24], 4);
    }
    else
    {
        // 如果不是 0x5A，说明是全新芯片或刚擦除过。
        // 此时维持代码中写死的默认值不变，并主动保存一次，完成初始化。
        Save_Params_To_EEPROM();
    }
}


// =========================================================================
// 函数名：Key_Menu_Adjust
// 功  能：多级菜单状态机与参数修改
// =========================================================================
void Key_Menu_Adjust(void)
{
    static uint8 key1_last = 1, key2_last = 1, key3_last = 1, key4_last = 1;
    uint8 key1_now, key2_now, key3_now, key4_now;

    key1_now = gpio_get_level(KEY1_PIN);
    key2_now = gpio_get_level(KEY2_PIN);
    key3_now = gpio_get_level(KEY3_PIN);
    key4_now = gpio_get_level(KEY4_PIN); 

    // ?? 【最高优先级保命机制】：只要在发车状态，按 KEY4 必定紧急停车！
    if(car_state == 1 && key4_now == 0 && key4_last == 1)
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY4_PIN) == 0)
        {
            car_state = 0;
            start_ramp_flag = 0;
            // ips114_clear(); // 如果你有清屏函数，可以在这里调用
            key4_last = key4_now;
            return; // 停车后直接退出本次按键检测
        }
    }

    // ----------------- 功能 1：切换光标 (KEY1 向下选择) -----------------
    if(key1_now == 0 && key1_last == 1)    
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY1_PIN) == 0)
        {
            cursor_index++; 
            if(cursor_index >= menu_max_items) cursor_index = 0; // 循环光标
        }
    }
    
    // ----------------- 功能 2：增加参数 / 向上选择 (KEY2) -----------------
    if(key2_now == 0 && key2_last == 1)    
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY2_PIN) == 0)
        {
            // 只有在最底层的调参界面，KEY2 才是“加参数”
            if(menu_state == MENU_SPEED) {
                if(cursor_index == 0) pid_loop_speed.Kp += 1.0f;
                if(cursor_index == 1) pid_loop_speed.Ki += 0.01f;
            }
            else if(menu_state == MENU_TURN) {
                if(cursor_index == 0) pid_motor_run.Kp += 0.1f;
                if(cursor_index == 1) pid_motor_run.Kd += 0.1f;
            }
            else if(menu_state == MENU_FORM) {
                if(cursor_index == 0) adc_set_differ.A += 0.1f;
                if(cursor_index == 1) adc_set_differ.B += 0.1f;
                if(cursor_index == 2) adc_set_differ.C += 0.1f;
            }
            else {
                // 如果在导航菜单里，KEY2 可以作为“光标向上”用，提升体验
                if(cursor_index == 0) cursor_index = menu_max_items - 1;
                else cursor_index--;
            }
        }
    }
    
    // ----------------- 功能 3：减小参数 (KEY3) -----------------
    if(key3_now == 0 && key3_last == 1)    
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY3_PIN) == 0)
        {
            if(menu_state == MENU_SPEED) {
                if(cursor_index == 0) { pid_loop_speed.Kp -= 1.0f;  if(pid_loop_speed.Kp < 0) pid_loop_speed.Kp = 0; }
                if(cursor_index == 1) { pid_loop_speed.Ki -= 0.01f; if(pid_loop_speed.Ki < 0) pid_loop_speed.Ki = 0; }
            }
            else if(menu_state == MENU_TURN) {
                if(cursor_index == 0) { pid_motor_run.Kp -= 0.1f; if(pid_motor_run.Kp < 0) pid_motor_run.Kp = 0; }
                if(cursor_index == 1) { pid_motor_run.Kd -= 0.1f; if(pid_motor_run.Kd < 0) pid_motor_run.Kd = 0; }
            }
            else if(menu_state == MENU_FORM) {
                if(cursor_index == 0) { adc_set_differ.A -= 0.1f; if(adc_set_differ.A < 0) adc_set_differ.A = 0; }
                if(cursor_index == 1) { adc_set_differ.B -= 0.1f; if(adc_set_differ.B < 0) adc_set_differ.B = 0; }
                if(cursor_index == 2) { adc_set_differ.C -= 0.1f; if(adc_set_differ.C < 0) adc_set_differ.C = 0; }
            }
        }
    }

    // ----------------- 功能 4：确认/进入下一级/发车 (KEY4) -----------------
    if(key4_now == 0 && key4_last == 1)    
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY4_PIN) == 0)
        {
            // 依据当前处于哪个界面，决定 KEY4 的行为
            switch(menu_state)
            {
                case MENU_MAIN: // 【主菜单】
                    if(cursor_index == 0)      { menu_state = MENU_TUNE_CATE; cursor_index = 0; menu_max_items = 4; } // 进调参
                    else if(cursor_index == 1) { menu_state = MENU_OBSERVE;   cursor_index = 0; menu_max_items = 1; } // 进观测
                    else if(cursor_index == 2) { 
                        // 按下发车！
                        car_state = 1; 
                        err_speed_L_last = 0; err_speed_L = 0;
                        err_speed_R_last = 0; err_speed_R = 0;
                        out_L = 0; out_R = 0;
                        direction_err1[0] = 0; direction_err1[1] = 0; direction_err1[2] = 0; 
												system_delay_ms(1000);
                        start_ramp_flag = 1;
                    }
                    break;

                case MENU_TUNE_CATE: // 【调参大类菜单】
                    if(cursor_index == 0)      { menu_state = MENU_SPEED; cursor_index = 0; menu_max_items = 3; } // 进速度环
                    else if(cursor_index == 1) { menu_state = MENU_TURN;  cursor_index = 0; menu_max_items = 3; } // 进转向环
                    else if(cursor_index == 2) { menu_state = MENU_FORM;  cursor_index = 0; menu_max_items = 4; } // 进差比和
                    else if(cursor_index == 3) { menu_state = MENU_MAIN;  cursor_index = 0; menu_max_items = 3; } // 返回上一级
                    break;

                case MENU_OBSERVE: // 【数据观测】
                    if(cursor_index == 0)      { menu_state = MENU_MAIN;  cursor_index = 0; menu_max_items = 3; } // 返回
                    break;

               case MENU_SPEED: // 【速度环底层】
                    if(cursor_index == 2) { 
                        Save_Params_To_EEPROM(); // <--- 退出时自动保存参数
                        menu_state = MENU_TUNE_CATE; 
                        cursor_index = 0; 
                        menu_max_items = 4; 
                    } 
                    break;
                case MENU_TURN:  // 【转向环底层】
                    if(cursor_index == 2) { 
                        Save_Params_To_EEPROM(); // <--- 退出时自动保存参数
                        menu_state = MENU_TUNE_CATE; 
                        cursor_index = 1; 
                        menu_max_items = 4; 
                    } 
                    break;
                case MENU_FORM:  // 【差比和底层】
                    if(cursor_index == 3) { 
                        Save_Params_To_EEPROM(); // <--- 退出时自动保存参数
                        menu_state = MENU_TUNE_CATE; 
                        cursor_index = 2; 
                        menu_max_items = 4; 
                    } 
                    break;
            }
        }
    }

    key1_last = key1_now; key2_last = key2_now; key3_last = key3_now; key4_last = key4_now;
}


// =========================================================================
// 函数名：UI_Display_Update
// 功  能：根据状态机渲染多级界面
// =========================================================================
void UI_Display_Update(void)
{
    char p[8]; // 用于存光标字符
    uint8 i;

    // 如果发车了，强制覆盖全屏显示运行状态
    if (start_ramp_flag == 1) {
        ips114_show_string(0, 3*16, "                  "); // 清空一些杂项
        ips114_show_string(0, 4*16, " >>> RUNNING! >>> ");
        ips114_show_string(0, 5*16, " PRESS KEY4 STOP  ");
        return; 
    }

    // 初始化光标数组全为空格
    for(i=0; i<8; i++) p[i] = ' ';
    p[cursor_index] = '>'; // 给当前选中项打上光标

    // 根据当前状态画UI
    switch(menu_state)
    {
        case MENU_MAIN:
            sprintf(disp_buf, "%c 1. Params        ", p[0]); ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c 2. Observe       ", p[1]); ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c 3. [START CAR!]  ", p[2]); ips114_show_string(0, 2*16, disp_buf);
            
            ips114_show_string(0, 3*16, "                  "); // 擦除多余行，防重影
            ips114_show_string(0, 4*16, "                  ");
            ips114_show_string(0, 5*16, "                  ");
            ips114_show_string(0, 6*16, "                  ");
            break;

        case MENU_TUNE_CATE:
            sprintf(disp_buf, "%c 1. Speed Loop    ", p[0]); ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c 2. Turn Loop     ", p[1]); ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c 3. A    B   C    ", p[2]); ips114_show_string(0, 2*16, disp_buf);
            sprintf(disp_buf, "%c <- Back          ", p[3]); ips114_show_string(0, 3*16, disp_buf);
            ips114_show_string(0, 4*16, "                  ");
            ips114_show_string(0, 5*16, "                  ");
            break;

        case MENU_SPEED:
            sprintf(disp_buf, "%c Spd Kp: %.1f     ", p[0], pid_loop_speed.Kp); ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c Spd Ki: %.2f     ", p[1], pid_loop_speed.Ki); ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c <- Back          ", p[2]);                    ips114_show_string(0, 2*16, disp_buf);
            ips114_show_string(0, 3*16, "                  ");
            break;

        case MENU_TURN:
            sprintf(disp_buf, "%c Turn Kp: %.2f    ", p[0], pid_motor_run.Kp);  ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c Turn Kd: %.2f    ", p[1], pid_motor_run.Kd);  ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c <- Back          ", p[2]);                    ips114_show_string(0, 2*16, disp_buf);
            ips114_show_string(0, 3*16, "                  ");
            break;

        case MENU_FORM:
            sprintf(disp_buf, "%c  A: %.1f         ", p[0], adc_set_differ.A);  ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c  B: %.1f         ", p[1], adc_set_differ.B);  ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c  C: %.2f         ", p[2], adc_set_differ.C);  ips114_show_string(0, 2*16, disp_buf);
            sprintf(disp_buf, "%c <- Back          ", p[3]);                    ips114_show_string(0, 3*16, disp_buf);
            break;

        case MENU_OBSERVE:            
            ips114_show_string(10, 16*0, "L :                ");
            ips114_show_string(10, 16*1, "LM:                ");
            ips114_show_string(10, 16*2, "RM:                ");
            ips114_show_string(10, 16*3, "R :                ");
						ips114_show_string(10, 16*4, "P :                ");
            
            ips114_show_float(40, 16*0, ADC_temp[0], 3, 1);
            ips114_show_float(40, 16*1, ADC_temp[1], 3, 1);
            ips114_show_float(40, 16*2, ADC_temp[3], 3, 1);
            ips114_show_float(40, 16*3, ADC_temp[2], 3, 1);
						ips114_show_float(40, 16*4, pitch, 3, 1);
            
           
            ips114_show_string(0, 16*5, "                  ");

           
            sprintf(disp_buf, "%c <- Back                  ", p[0]); 
            ips114_show_string(0, 16*5, disp_buf); 
            break;
           
    }
}









