#include "menu.h"
#include "string.h"

#define MENU_MAIN       0
#define MENU_TUNE_CATE  1
#define MENU_OBSERVE    2
#define MENU_SPEED      3
#define MENU_TURN       4
#define MENU_FORM       5

uint8 menu_state = MENU_MAIN;
uint8 cursor_index = 0;
uint8 menu_max_items = 3;

char disp_buf[32];
uint8 car_state = 0;
uint8 start_ramp_flag = 0;

#define EEPROM_PARAM_ADDR   0x0000
#define EEPROM_INIT_FLAG    0x5A

void Save_Params_To_EEPROM(void)
{
    uint8 save_buf[29];

    save_buf[0] = EEPROM_INIT_FLAG;

    memcpy(&save_buf[1],  &pid_loop_speed.Kp, 4);
    memcpy(&save_buf[5],  &pid_loop_speed.Ki, 4);
    memcpy(&save_buf[9],  &pid_motor_run.Kp,  4);
    memcpy(&save_buf[13], &pid_motor_run.Kd,  4);
    memcpy(&save_buf[17], &adc_set_differ.A,  4);
    memcpy(&save_buf[21], &adc_set_differ.B,  4);
    memcpy(&save_buf[25], &adc_set_differ.C,  4);

    extern_iap_write_buff(EEPROM_PARAM_ADDR, save_buf, 29);
}

void Load_Params_From_EEPROM(void)
{
    uint8 flag = 0;

    flag = iap_read_byte(EEPROM_PARAM_ADDR);

    if(flag == EEPROM_INIT_FLAG)
    {
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
        Save_Params_To_EEPROM();
    }
}

void Key_Menu_Adjust(void)
{
    static uint8 key1_last = 1, key2_last = 1, key3_last = 1, key4_last = 1, key5_last = 1;
    uint8 key1_now, key2_now, key3_now, key4_now, key5_now;

    key1_now = gpio_get_level(KEY1_PIN);
    key2_now = gpio_get_level(KEY2_PIN);
    key3_now = gpio_get_level(KEY3_PIN);
    key4_now = gpio_get_level(KEY4_PIN);
    key5_now = gpio_get_level(KEY5_PIN);

    // KEY5: stop car when running
    if(car_state == 1 && key5_now == 0 && key5_last == 1)
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY5_PIN) == 0)
        {
            car_state = 0;
            start_ramp_flag = 0;
            key5_last = key5_now;
            return;
        }
    }

    // KEY1: cycle cursor
    if(key1_now == 0 && key1_last == 1)
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY1_PIN) == 0)
        {
            cursor_index++;
            if(cursor_index >= menu_max_items) cursor_index = 0;
        }
    }

    // KEY4: back to parent menu (auto-save in tuning pages)
    if(key4_now == 0 && key4_last == 1)
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY4_PIN) == 0)
        {
            switch(menu_state)
            {
                case MENU_MAIN:
                    break;

                case MENU_TUNE_CATE:
                    menu_state = MENU_MAIN;
                    cursor_index = 0;
                    menu_max_items = 3;
                    break;

                case MENU_OBSERVE:
                    menu_state = MENU_MAIN;
                    cursor_index = 0;
                    menu_max_items = 3;
                    break;

                case MENU_SPEED:
                    Save_Params_To_EEPROM();
                    menu_state = MENU_TUNE_CATE;
                    cursor_index = 0;
                    menu_max_items = 3;
                    break;

                case MENU_TURN:
                    Save_Params_To_EEPROM();
                    menu_state = MENU_TUNE_CATE;
                    cursor_index = 1;
                    menu_max_items = 3;
                    break;

                case MENU_FORM:
                    Save_Params_To_EEPROM();
                    menu_state = MENU_TUNE_CATE;
                    cursor_index = 2;
                    menu_max_items = 3;
                    break;
            }
        }
    }

    // KEY2: increase param / cursor up
    if(key2_now == 0 && key2_last == 1)
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY2_PIN) == 0)
        {
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
                if(cursor_index == 0) cursor_index = menu_max_items - 1;
                else cursor_index--;
            }
        }
    }

    // KEY3: decrease param
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

    // KEY5: confirm / enter / start car
    if(key5_now == 0 && key5_last == 1)
    {
        system_delay_ms(5);
        if(gpio_get_level(KEY5_PIN) == 0)
        {
            switch(menu_state)
            {
                case MENU_MAIN:
                    if(cursor_index == 0) {
                        car_state = 1;
                        err_speed_L_last = 0; err_speed_L = 0;
                        err_speed_R_last = 0; err_speed_R = 0;
                        out_L = 0; out_R = 0;
                        direction_err1[0] = 0; direction_err1[1] = 0; direction_err1[2] = 0;
                        system_delay_ms(500);
                        start_ramp_flag = 1;
                    }
                    else if(cursor_index == 1) { menu_state = MENU_TUNE_CATE; cursor_index = 0; menu_max_items = 3; }
                    else if(cursor_index == 2) { menu_state = MENU_OBSERVE;   cursor_index = 0; menu_max_items = 1; }
                    break;

                case MENU_TUNE_CATE:
                    if(cursor_index == 0)      { menu_state = MENU_SPEED; cursor_index = 0; menu_max_items = 2; }
                    else if(cursor_index == 1) { menu_state = MENU_TURN;  cursor_index = 0; menu_max_items = 2; }
                    else if(cursor_index == 2) { menu_state = MENU_FORM;  cursor_index = 0; menu_max_items = 3; }
                    break;

                case MENU_OBSERVE:
                    break;

                case MENU_SPEED:
                case MENU_TURN:
                case MENU_FORM:
                    break;
            }
        }
    }

    key1_last = key1_now; key2_last = key2_now; key3_last = key3_now; key4_last = key4_now; key5_last = key5_now;
}

void UI_Display_Update(void)
{
    char p[8];
    uint8 i;

    if (start_ramp_flag == 1) {
        ips114_show_string(0, 3*16, "                  ");
        ips114_show_string(0, 4*16, " >>> RUNNING! >>> ");
        ips114_show_string(0, 5*16, " PRESS KEY5 STOP  ");
        return;
    }

    for(i=0; i<8; i++) p[i] = ' ';
    p[cursor_index] = '>';

    switch(menu_state)
    {
        case MENU_MAIN:
            sprintf(disp_buf, "%c 1. [START CAR!]  ", p[0]); ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c 2. Params        ", p[1]); ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c 3. Observe       ", p[2]); ips114_show_string(0, 2*16, disp_buf);

            ips114_show_string(0, 3*16, "                  ");
            ips114_show_string(0, 4*16, "                  ");
            ips114_show_string(0, 5*16, "                  ");
            ips114_show_string(0, 6*16, "                  ");
            break;

        case MENU_TUNE_CATE:
            sprintf(disp_buf, "%c 1. Speed Loop    ", p[0]); ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c 2. Turn Loop     ", p[1]); ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c 3. A    B   C    ", p[2]); ips114_show_string(0, 2*16, disp_buf);
            ips114_show_string(0, 3*16, " KEY4: Back        ");
            ips114_show_string(0, 4*16, "                  ");
            ips114_show_string(0, 5*16, "                  ");
            break;

        case MENU_SPEED:
            sprintf(disp_buf, "%c Spd Kp: %.1f     ", p[0], pid_loop_speed.Kp); ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c Spd Ki: %.2f     ", p[1], pid_loop_speed.Ki); ips114_show_string(0, 1*16, disp_buf);
            ips114_show_string(0, 2*16, " KEY4: Back & Save ");
            ips114_show_string(0, 3*16, "                  ");
            break;

        case MENU_TURN:
            sprintf(disp_buf, "%c Turn Kp: %.2f    ", p[0], pid_motor_run.Kp);  ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c Turn Kd: %.2f    ", p[1], pid_motor_run.Kd);  ips114_show_string(0, 1*16, disp_buf);
            ips114_show_string(0, 2*16, " KEY4: Back & Save ");
            ips114_show_string(0, 3*16, "                  ");
            break;

        case MENU_FORM:
            sprintf(disp_buf, "%c  A: %.1f         ", p[0], adc_set_differ.A);  ips114_show_string(0, 0*16, disp_buf);
            sprintf(disp_buf, "%c  B: %.1f         ", p[1], adc_set_differ.B);  ips114_show_string(0, 1*16, disp_buf);
            sprintf(disp_buf, "%c  C: %.2f         ", p[2], adc_set_differ.C);  ips114_show_string(0, 2*16, disp_buf);
            ips114_show_string(0, 3*16, " KEY4: Back & Save ");
            break;

        case MENU_OBSERVE:
            ips114_show_string(10, 16*0, "L :                ");
            ips114_show_string(10, 16*1, "LM:                ");
            ips114_show_string(10, 16*2, "RM:                ");
            ips114_show_string(10, 16*3, "R :                ");
            ips114_show_string(10, 16*4, "Y :                ");
            ips114_show_string(10, 16*5, "R :                ");

            ips114_show_float(40, 16*0, ADC_temp[0], 3, 1);
            ips114_show_float(40, 16*1, ADC_temp[1], 3, 1);
            ips114_show_float(40, 16*2, ADC_temp[3], 3, 1);
            ips114_show_float(40, 16*3, ADC_temp[2], 3, 1);
            ips114_show_float(40, 16*4, imu660rc_yaw, 4, 1);
            ips114_show_float(40, 16*5, imu660rc_roll, 4, 1);

            ips114_show_string(0, 16*6, " KEY4: Back        ");
            break;
    }
}
