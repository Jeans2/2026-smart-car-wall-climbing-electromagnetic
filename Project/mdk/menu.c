#include "menu.h"
#include "string.h"

#define MENU_MAIN       0
#define MENU_TUNE_CATE  1
#define MENU_OBSERVE    2
#define MENU_SPEED      3
#define MENU_TURN       4
#define MENU_FORM       5
#define MENU_RING       6


uint8 menu_state = MENU_MAIN;
uint8 cursor_index = 0;
uint8 menu_max_items = 3;

char disp_buf[32];
uint8 car_state = 0;
volatile uint8 start_ramp_flag = 0;

#define EEPROM_PARAM_ADDR   0x0000
#define EEPROM_INIT_FLAG    0x62
#define EEPROM_RING_STOP_FLAG 0x61
#define EEPROM_RING_SEQ_FLAG 0x60
#define EEPROM_RING4_FLAG   0x5F
#define EEPROM_QQB_FLAG     0x5E
#define EEPROM_EXTENDED_FLAG 0x5D
#define EEPROM_SINGLE_SPEED_FLAG 0x5B
#define EEPROM_YAW_RATE_FLAG 0x5C
#define EEPROM_OLD_FLAG     0x5A

static uint8 menu_ring_digit_get(uint8 pos);
static void menu_ring_digit_set(uint8 pos, uint8 val);
static void menu_ring_show_seq(void);

static uint32 menu_pow10(uint8 n)
{
    uint32 dat = 1;

    while (n > 0)
    {
        dat *= 10;
        n--;
    }

    return dat;
}

static void menu_ring_params_limit(void)
{
    uint8 i;
    uint32 clean_code = 0;

    if (ring_plan_laps < 1) ring_plan_laps = 1;
    if (ring_plan_laps > 9) ring_plan_laps = 9;
    if (ring_plan_seq_len < 1) ring_plan_seq_len = 1;
    if (ring_plan_seq_len > 8) ring_plan_seq_len = 8;
    if (ring_plan_seq_pos < 1) ring_plan_seq_pos = 1;
    if (ring_plan_seq_pos > ring_plan_seq_len) ring_plan_seq_pos = ring_plan_seq_len;
    if (ring_plan_stop_action > 2) ring_plan_stop_action = 2;

    for (i = 1; i <= ring_plan_seq_len; i++)
    {
        clean_code = clean_code * 10 + menu_ring_digit_get(i);
    }
    ring_plan_seq_code = clean_code;
}

static uint8 menu_ring_digit_get(uint8 pos)
{
    uint8 len;
    uint8 digit;
    uint32 divisor;

    len = ring_plan_seq_len;
    if (len < 1) len = 1;
    if (len > 8) len = 8;
    if (pos < 1 || pos > len) return 0;

    divisor = menu_pow10(len - pos);
    digit = (uint8)((ring_plan_seq_code / divisor) % 10);
    if (digit > 2) digit = 0;
    return digit;
}

static void menu_ring_digit_set(uint8 pos, uint8 val)
{
    uint8 i;
    uint8 digit;
    uint32 new_code = 0;

    if (val > 2) val = 0;
    menu_ring_params_limit();

    for (i = 1; i <= ring_plan_seq_len; i++)
    {
        digit = (i == pos) ? val : menu_ring_digit_get(i);
        new_code = new_code * 10 + digit;
    }

    ring_plan_seq_code = new_code;
}

void Save_Params_To_EEPROM(void)
{
    uint8 save_buf[61];

    menu_ring_params_limit();

    save_buf[0] = EEPROM_INIT_FLAG;

    memcpy(&save_buf[1],  &pid_loop_speed_L.Kp, 4);
    memcpy(&save_buf[5],  &pid_loop_speed_L.Ki, 4);
    memcpy(&save_buf[9],  &pid_loop_speed_L.Kd, 4);
    memcpy(&save_buf[13], &pid_loop_speed_R.Kp, 4);
    memcpy(&save_buf[17], &pid_loop_speed_R.Ki, 4);
    memcpy(&save_buf[21], &pid_loop_speed_R.Kd, 4);
    memcpy(&save_buf[25], &pid_motor_run.Kp,  4);
    memcpy(&save_buf[29], &pid_motor_run.Ki,  4);
    memcpy(&save_buf[33], &pid_motor_run.Kd,  4);
    memcpy(&save_buf[37], &adc_set_differ.A,  4);
    memcpy(&save_buf[41], &adc_set_differ.B,  4);
    memcpy(&save_buf[45], &adc_set_differ.C,  4);
    memcpy(&save_buf[49], &qqb_low_speed_delay, 2);
    memcpy(&save_buf[51], &qqb_low_speed_target, 2);
    save_buf[53] = ring_plan_laps;
    save_buf[54] = ring_plan_seq_len;
    save_buf[55] = ring_plan_seq_pos;
    memcpy(&save_buf[56], &ring_plan_seq_code, 4);
    save_buf[60] = ring_plan_stop_action;

    extern_iap_write_buff(EEPROM_PARAM_ADDR, save_buf, 61);
}

void Load_Params_From_EEPROM(void)
{
    uint8 flag = 0;

    flag = iap_read_byte(EEPROM_PARAM_ADDR);

    if(flag == EEPROM_INIT_FLAG || flag == EEPROM_RING_STOP_FLAG || flag == EEPROM_RING_SEQ_FLAG)
    {
        uint8 read_buf[60];
        uint8 read_len;

        read_len = (flag == EEPROM_RING_SEQ_FLAG) ? 59 : 60;
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_buf, read_len);

        memcpy(&pid_loop_speed_L.Kp, &read_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_buf[4],  4);
        memcpy(&pid_loop_speed_L.Kd, &read_buf[8],  4);
        memcpy(&pid_loop_speed_R.Kp, &read_buf[12], 4);
        memcpy(&pid_loop_speed_R.Ki, &read_buf[16], 4);
        memcpy(&pid_loop_speed_R.Kd, &read_buf[20], 4);
        memcpy(&pid_motor_run.Kp,    &read_buf[24], 4);
        memcpy(&pid_motor_run.Ki,    &read_buf[28], 4);
        memcpy(&pid_motor_run.Kd,    &read_buf[32], 4);
        memcpy(&adc_set_differ.A,    &read_buf[36], 4);
        memcpy(&adc_set_differ.B,    &read_buf[40], 4);
        memcpy(&adc_set_differ.C,    &read_buf[44], 4);
        memcpy(&qqb_low_speed_delay, &read_buf[48], 2);
        memcpy(&qqb_low_speed_target, &read_buf[50], 2);
        if (flag == EEPROM_INIT_FLAG || flag == EEPROM_RING_STOP_FLAG)
        {
            ring_plan_laps = read_buf[52];
            ring_plan_seq_len = read_buf[53];
            ring_plan_seq_pos = read_buf[54];
            memcpy(&ring_plan_seq_code, &read_buf[55], 4);
            ring_plan_stop_action = read_buf[59];
            if (flag == EEPROM_RING_STOP_FLAG)
            {
                if (ring_plan_seq_len == 2 && ring_plan_seq_code == 2)
                {
                    ring_plan_seq_len = 3;
                    ring_plan_seq_code = 20;
                }
                Save_Params_To_EEPROM();
            }
        }
        else
        {
            ring_plan_laps = 2;
            ring_plan_seq_len = 3;
            ring_plan_seq_pos = 1;
            ring_plan_seq_code = 20;
            ring_plan_stop_action = 2;
            Save_Params_To_EEPROM();
        }
    }
    else if(flag == EEPROM_RING4_FLAG)
    {
        uint8 read_buf[59];
        uint8 i;
        uint8 old_total;
        uint8 digit;
        uint32 old_code = 0;

        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_buf, 59);

        memcpy(&pid_loop_speed_L.Kp, &read_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_buf[4],  4);
        memcpy(&pid_loop_speed_L.Kd, &read_buf[8],  4);
        memcpy(&pid_loop_speed_R.Kp, &read_buf[12], 4);
        memcpy(&pid_loop_speed_R.Ki, &read_buf[16], 4);
        memcpy(&pid_loop_speed_R.Kd, &read_buf[20], 4);
        memcpy(&pid_motor_run.Kp,    &read_buf[24], 4);
        memcpy(&pid_motor_run.Ki,    &read_buf[28], 4);
        memcpy(&pid_motor_run.Kd,    &read_buf[32], 4);
        memcpy(&adc_set_differ.A,    &read_buf[36], 4);
        memcpy(&adc_set_differ.B,    &read_buf[40], 4);
        memcpy(&adc_set_differ.C,    &read_buf[44], 4);
        memcpy(&qqb_low_speed_delay, &read_buf[48], 2);
        memcpy(&qqb_low_speed_target, &read_buf[50], 2);
        ring_plan_laps = read_buf[52];

        old_total = read_buf[53] + read_buf[54];
        if (old_total < 1) old_total = 1;
        if (old_total > 4) old_total = 4;
        ring_plan_seq_len = old_total;
        ring_plan_seq_pos = 1;

        for (i = 0; i < old_total; i++)
        {
            digit = read_buf[55 + i];
            if (digit > 2) digit = 0;
            old_code = old_code * 10 + digit;
        }
        ring_plan_seq_code = old_code;

        Save_Params_To_EEPROM();
    }
    else if(flag == EEPROM_QQB_FLAG)
    {
        uint8 read_buf[52];
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_buf, 52);

        memcpy(&pid_loop_speed_L.Kp, &read_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_buf[4],  4);
        memcpy(&pid_loop_speed_L.Kd, &read_buf[8],  4);
        memcpy(&pid_loop_speed_R.Kp, &read_buf[12], 4);
        memcpy(&pid_loop_speed_R.Ki, &read_buf[16], 4);
        memcpy(&pid_loop_speed_R.Kd, &read_buf[20], 4);
        memcpy(&pid_motor_run.Kp,    &read_buf[24], 4);
        memcpy(&pid_motor_run.Ki,    &read_buf[28], 4);
        memcpy(&pid_motor_run.Kd,    &read_buf[32], 4);
        memcpy(&adc_set_differ.A,    &read_buf[36], 4);
        memcpy(&adc_set_differ.B,    &read_buf[40], 4);
        memcpy(&adc_set_differ.C,    &read_buf[44], 4);
        memcpy(&qqb_low_speed_delay, &read_buf[48], 2);
        memcpy(&qqb_low_speed_target, &read_buf[50], 2);

        Save_Params_To_EEPROM();
    }
    else if(flag == EEPROM_EXTENDED_FLAG)
    {
        uint8 read_ext_buf[48];
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_ext_buf, 48);

        memcpy(&pid_loop_speed_L.Kp, &read_ext_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_ext_buf[4],  4);
        memcpy(&pid_loop_speed_L.Kd, &read_ext_buf[8],  4);
        memcpy(&pid_loop_speed_R.Kp, &read_ext_buf[12], 4);
        memcpy(&pid_loop_speed_R.Ki, &read_ext_buf[16], 4);
        memcpy(&pid_loop_speed_R.Kd, &read_ext_buf[20], 4);
        memcpy(&pid_motor_run.Kp,    &read_ext_buf[24], 4);
        memcpy(&pid_motor_run.Ki,    &read_ext_buf[28], 4);
        memcpy(&pid_motor_run.Kd,    &read_ext_buf[32], 4);
        memcpy(&adc_set_differ.A,    &read_ext_buf[36], 4);
        memcpy(&adc_set_differ.B,    &read_ext_buf[40], 4);
        memcpy(&adc_set_differ.C,    &read_ext_buf[44], 4);

        Save_Params_To_EEPROM();
    }
    else if(flag == EEPROM_SINGLE_SPEED_FLAG)
    {
        uint8 read_buf[36];
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_buf, 36);

        memcpy(&pid_loop_speed_L.Kp, &read_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_buf[4],  4);
        memcpy(&pid_loop_speed_L.Kd, &read_buf[8],  4);
        pid_loop_speed_R.Kp = pid_loop_speed_L.Kp;
        pid_loop_speed_R.Ki = pid_loop_speed_L.Ki;
        pid_loop_speed_R.Kd = pid_loop_speed_L.Kd;
        memcpy(&pid_motor_run.Kp,    &read_buf[12], 4);
        memcpy(&pid_motor_run.Ki,    &read_buf[16], 4);
        memcpy(&pid_motor_run.Kd,    &read_buf[20], 4);
        memcpy(&adc_set_differ.A,    &read_buf[24], 4);
        memcpy(&adc_set_differ.B,    &read_buf[28], 4);
        memcpy(&adc_set_differ.C,    &read_buf[32], 4);

        Save_Params_To_EEPROM();
    }
    else if(flag == EEPROM_YAW_RATE_FLAG)
    {
        uint8 read_yaw_buf[48];
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_yaw_buf, 48);

        memcpy(&pid_loop_speed_L.Kp, &read_yaw_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_yaw_buf[4],  4);
        memcpy(&pid_loop_speed_L.Kd, &read_yaw_buf[8],  4);
        pid_loop_speed_R.Kp = pid_loop_speed_L.Kp;
        pid_loop_speed_R.Ki = pid_loop_speed_L.Ki;
        pid_loop_speed_R.Kd = pid_loop_speed_L.Kd;
        memcpy(&pid_motor_run.Kp,    &read_yaw_buf[12], 4);
        memcpy(&pid_motor_run.Ki,    &read_yaw_buf[16], 4);
        memcpy(&pid_motor_run.Kd,    &read_yaw_buf[20], 4);
        memcpy(&adc_set_differ.A,    &read_yaw_buf[36], 4);
        memcpy(&adc_set_differ.B,    &read_yaw_buf[40], 4);
        memcpy(&adc_set_differ.C,    &read_yaw_buf[44], 4);

        Save_Params_To_EEPROM();
    }
    else if(flag == EEPROM_OLD_FLAG)
    {
        uint8 read_old_buf[28];
        iap_read_buff(EEPROM_PARAM_ADDR + 1, read_old_buf, 28);

        memcpy(&pid_loop_speed_L.Kp, &read_old_buf[0],  4);
        memcpy(&pid_loop_speed_L.Ki, &read_old_buf[4],  4);
        pid_loop_speed_R.Kp = pid_loop_speed_L.Kp;
        pid_loop_speed_R.Ki = pid_loop_speed_L.Ki;
        pid_loop_speed_R.Kd = pid_loop_speed_L.Kd;
        memcpy(&pid_motor_run.Kp,    &read_old_buf[8],  4);
        memcpy(&pid_motor_run.Kd,    &read_old_buf[12], 4);
        memcpy(&adc_set_differ.A,    &read_old_buf[16], 4);
        memcpy(&adc_set_differ.B,    &read_old_buf[20], 4);
        memcpy(&adc_set_differ.C,    &read_old_buf[24], 4);

        Save_Params_To_EEPROM();
    }
    else
    {
        Save_Params_To_EEPROM();
    }

    menu_ring_params_limit();
}

void Key_Menu_Adjust(void)
{
    static uint8 key1_last = 1, key2_last = 1, key3_last = 1, key4_last = 1, key5_last = 1;
    uint8 key1_now, key2_now, key3_now, key4_now, key5_now;
    uint8 ring_val;

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
                    menu_max_items = 4;
                    break;

                case MENU_TURN:
                    Save_Params_To_EEPROM();
                    menu_state = MENU_TUNE_CATE;
                    cursor_index = 1;
                    menu_max_items = 4;
                    break;

                case MENU_FORM:
                    Save_Params_To_EEPROM();
                    menu_state = MENU_TUNE_CATE;
                    cursor_index = 2;
                    menu_max_items = 4;
                    break;

                case MENU_RING:
                    Save_Params_To_EEPROM();
                    menu_state = MENU_TUNE_CATE;
                    cursor_index = 3;
                    menu_max_items = 4;
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
                if(cursor_index == 0) pid_loop_speed_L.Kp += 1.0f;
                if(cursor_index == 1) pid_loop_speed_L.Ki += 0.01f;
                if(cursor_index == 2) pid_loop_speed_L.Kd += 0.1f;
                if(cursor_index == 3) pid_loop_speed_R.Kp += 1.0f;
                if(cursor_index == 4) pid_loop_speed_R.Ki += 0.01f;
                if(cursor_index == 5) pid_loop_speed_R.Kd += 0.1f;
            }
            else if(menu_state == MENU_TURN) {
                if(cursor_index == 0) pid_motor_run.Kp += 0.1f;
                if(cursor_index == 1) pid_motor_run.Ki += 0.01f;
                if(cursor_index == 2) pid_motor_run.Kd += 0.1f;
            }
            else if(menu_state == MENU_FORM) {
                if(cursor_index == 0) adc_set_differ.A += 0.1f;
                if(cursor_index == 1) adc_set_differ.B += 0.1f;
                if(cursor_index == 2) adc_set_differ.C += 0.1f;
            }
            else if(menu_state == MENU_RING) {
                if(cursor_index == 0 && ring_plan_laps < 9) ring_plan_laps++;
                if(cursor_index == 1 && ring_plan_seq_len < 8) ring_plan_seq_len++;
                if(cursor_index == 2) { ring_plan_seq_pos++; if(ring_plan_seq_pos > ring_plan_seq_len) ring_plan_seq_pos = 1; }
                if(cursor_index == 3) { ring_val = menu_ring_digit_get(ring_plan_seq_pos); ring_val++; if(ring_val > 2) ring_val = 0; menu_ring_digit_set(ring_plan_seq_pos, ring_val); }
                if(cursor_index == 4) { ring_plan_stop_action++; if(ring_plan_stop_action > 2) ring_plan_stop_action = 0; }
                menu_ring_params_limit();
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
                if(cursor_index == 0) { pid_loop_speed_L.Kp -= 1.0f;  if(pid_loop_speed_L.Kp < 0) pid_loop_speed_L.Kp = 0; }
                if(cursor_index == 1) { pid_loop_speed_L.Ki -= 0.01f; if(pid_loop_speed_L.Ki < 0) pid_loop_speed_L.Ki = 0; }
                if(cursor_index == 2) { pid_loop_speed_L.Kd -= 0.1f;  if(pid_loop_speed_L.Kd < 0) pid_loop_speed_L.Kd = 0; }
                if(cursor_index == 3) { pid_loop_speed_R.Kp -= 1.0f;  if(pid_loop_speed_R.Kp < 0) pid_loop_speed_R.Kp = 0; }
                if(cursor_index == 4) { pid_loop_speed_R.Ki -= 0.01f; if(pid_loop_speed_R.Ki < 0) pid_loop_speed_R.Ki = 0; }
                if(cursor_index == 5) { pid_loop_speed_R.Kd -= 0.1f;  if(pid_loop_speed_R.Kd < 0) pid_loop_speed_R.Kd = 0; }
            }
            else if(menu_state == MENU_TURN) {
                if(cursor_index == 0) { pid_motor_run.Kp -= 0.1f; if(pid_motor_run.Kp < 0) pid_motor_run.Kp = 0; }
                if(cursor_index == 1) { pid_motor_run.Ki -= 0.01f; if(pid_motor_run.Ki < 0) pid_motor_run.Ki = 0; }
                if(cursor_index == 2) { pid_motor_run.Kd -= 0.1f;  if(pid_motor_run.Kd < 0) pid_motor_run.Kd = 0; }
            }
            else if(menu_state == MENU_FORM) {
                if(cursor_index == 0) { adc_set_differ.A -= 0.1f; if(adc_set_differ.A < 0) adc_set_differ.A = 0; }
                if(cursor_index == 1) { adc_set_differ.B -= 0.1f; if(adc_set_differ.B < 0) adc_set_differ.B = 0; }
                if(cursor_index == 2) { adc_set_differ.C -= 0.1f; if(adc_set_differ.C < 0) adc_set_differ.C = 0; }
            }
            else if(menu_state == MENU_RING) {
                if(cursor_index == 0 && ring_plan_laps > 1) ring_plan_laps--;
                if(cursor_index == 1 && ring_plan_seq_len > 1) ring_plan_seq_len--;
                if(cursor_index == 2) { if(ring_plan_seq_pos <= 1) ring_plan_seq_pos = ring_plan_seq_len; else ring_plan_seq_pos--; }
                if(cursor_index == 3) { ring_val = menu_ring_digit_get(ring_plan_seq_pos); if(ring_val == 0) ring_val = 2; else ring_val--; menu_ring_digit_set(ring_plan_seq_pos, ring_val); }
                if(cursor_index == 4) { if(ring_plan_stop_action == 0) ring_plan_stop_action = 2; else ring_plan_stop_action--; }
                menu_ring_params_limit();
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
                        pid_control_reset();
                        xunji_state_reset();
                        system_delay_ms(500);
                        start_ramp_flag = 1;
                    }
                    else if(cursor_index == 1) { menu_state = MENU_TUNE_CATE; cursor_index = 0; menu_max_items = 4; }
                    else if(cursor_index == 2) { menu_state = MENU_OBSERVE;   cursor_index = 0; menu_max_items = 1; }
                    break;

                case MENU_TUNE_CATE:
                    if(cursor_index == 0)      { menu_state = MENU_SPEED; cursor_index = 0; menu_max_items = 6; }
                    else if(cursor_index == 1) { menu_state = MENU_TURN;  cursor_index = 0; menu_max_items = 3; }
                    else if(cursor_index == 2) { menu_state = MENU_FORM;  cursor_index = 0; menu_max_items = 3; }
                    else if(cursor_index == 3) { menu_state = MENU_RING;  cursor_index = 0; menu_max_items = 5; }
                    break;

                case MENU_OBSERVE:
                    break;

                case MENU_SPEED:
                case MENU_TURN:
                case MENU_FORM:
                case MENU_RING:
                    break;
            }
        }
    }

    key1_last = key1_now; key2_last = key2_now; key3_last = key3_now; key4_last = key4_now; key5_last = key5_now;
}
static void menu_clear_line(uint8 row)
{
    uint8 i;

    for(i = 0; i < 16; i++)
    {
        ips114_show_char(i * 8, row * 16, ' ');
    }
}

static void menu_clear_all(void)
{
    uint8 i;

    for(i = 0; i < 8; i++)
    {
        menu_clear_line(i);
    }
}

static void menu_ring_show_seq(void)
{
    uint8 i;

    menu_ring_params_limit();
    for (i = 0; i < 8; i++)
    {
        disp_buf[i] = ' ';
    }
    disp_buf[8] = '\0';
    ips114_show_string(40, 5 * 16, disp_buf);

    for (i = 0; i < ring_plan_seq_len; i++)
    {
        disp_buf[i] = (char)('0' + menu_ring_digit_get(i + 1));
    }
    disp_buf[ring_plan_seq_len] = '\0';
    ips114_show_string(40, 5 * 16, disp_buf);
}
static void menu_show_label(uint8 row, char mark, const char dat[])
{
    ips114_show_char(0, row * 16, mark);
    ips114_show_string(8, row * 16, dat);
}

void UI_Display_Update(void)
{
    char p[8];
    uint8 i;
    uint8 redraw;
    static uint8 observe_drawn = 0;
    static uint8 running_drawn = 0;
    static uint8 last_menu_state = 0xFF;
    static uint8 last_cursor_index = 0xFF;

    if (start_ramp_flag == 1)
    {
        if (running_drawn == 0)
        {
            menu_clear_all();
            ips114_show_string(0, 4 * 16, "RUN");
            ips114_show_string(0, 5 * 16, "K5 STOP");
            running_drawn = 1;
            last_menu_state = 0xFF;
            last_cursor_index = 0xFF;
        }
        return;
    }

    running_drawn = 0;
    redraw = (menu_state != last_menu_state || cursor_index != last_cursor_index);

    for(i = 0; i < 8; i++) p[i] = ' ';
    p[cursor_index] = '>';

    if (menu_state != MENU_OBSERVE)
    {
        observe_drawn = 0;
        if (redraw)
        {
            menu_clear_all();
        }
    }

    switch(menu_state)
    {
        case MENU_MAIN:
            if (redraw)
            {
                menu_show_label(0, p[0], "Sta");
                menu_show_label(1, p[1], "Par");
                menu_show_label(2, p[2], "Obs");
            }
            break;

        case MENU_TUNE_CATE:
            if (redraw)
            {
                menu_show_label(0, p[0], "Spd");
                menu_show_label(1, p[1], "Trn");
                menu_show_label(2, p[2], "ABC");
                menu_show_label(3, p[3], "Ring");
                ips114_show_string(0, 4 * 16, "K4");
            }
            break;

        case MENU_SPEED:
            if (redraw)
            {
                menu_show_label(0, p[0], "LKp");
                menu_show_label(1, p[1], "LKi");
                menu_show_label(2, p[2], "LKd");
                menu_show_label(3, p[3], "RKp");
                menu_show_label(4, p[4], "RKi");
                menu_show_label(5, p[5], "RKd");
                ips114_show_string(0, 6 * 16, "K4");
            }
            ips114_show_float(40, 0 * 16, pid_loop_speed_L.Kp, 3, 1);
            ips114_show_float(40, 1 * 16, pid_loop_speed_L.Ki, 3, 2);
            ips114_show_float(40, 2 * 16, pid_loop_speed_L.Kd, 3, 1);
            ips114_show_float(40, 3 * 16, pid_loop_speed_R.Kp, 3, 1);
            ips114_show_float(40, 4 * 16, pid_loop_speed_R.Ki, 3, 2);
            ips114_show_float(40, 5 * 16, pid_loop_speed_R.Kd, 3, 1);
            break;

        case MENU_TURN:
            if (redraw)
            {
                menu_show_label(0, p[0], "TKp");
                menu_show_label(1, p[1], "TKi");
                menu_show_label(2, p[2], "TKd");
                ips114_show_string(0, 3 * 16, "K4");
            }
            ips114_show_float(40, 0 * 16, pid_motor_run.Kp, 3, 2);
            ips114_show_float(40, 1 * 16, pid_motor_run.Ki, 3, 2);
            ips114_show_float(40, 2 * 16, pid_motor_run.Kd, 3, 2);
            break;

        case MENU_FORM:
            if (redraw)
            {
                menu_show_label(0, p[0], "A");
                menu_show_label(1, p[1], "B");
                menu_show_label(2, p[2], "C");
                ips114_show_string(0, 3 * 16, "K4");
            }
            ips114_show_float(40, 0 * 16, adc_set_differ.A, 3, 1);
            ips114_show_float(40, 1 * 16, adc_set_differ.B, 3, 1);
            ips114_show_float(40, 2 * 16, adc_set_differ.C, 3, 2);
            break;
        case MENU_RING:
            if (redraw)
            {
                menu_show_label(0, p[0], "Lap");
                menu_show_label(1, p[1], "Len");
                menu_show_label(2, p[2], "Pos");
                menu_show_label(3, p[3], "Val");
                menu_show_label(4, p[4], "Stop");
                ips114_show_string(8, 5 * 16, "Seq");
                ips114_show_string(0, 6 * 16, "0Li 1L 2R");
                ips114_show_string(0, 7 * 16, "K4");
            }
            ips114_show_uint8(40, 0 * 16, ring_plan_laps);
            ips114_show_uint8(40, 1 * 16, ring_plan_seq_len);
            ips114_show_uint8(40, 2 * 16, ring_plan_seq_pos);
            ips114_show_uint8(40, 3 * 16, menu_ring_digit_get(ring_plan_seq_pos));
            ips114_show_uint8(48, 4 * 16, ring_plan_stop_action);
            menu_ring_show_seq();
            break;
        case MENU_OBSERVE:
            if (observe_drawn == 0 || redraw)
            {
                menu_clear_all();
                ips114_show_string(10, 16 * 0, "L");
                ips114_show_string(10, 16 * 1, "LM");
                ips114_show_string(10, 16 * 2, "RM");
                ips114_show_string(10, 16 * 3, "R");
                ips114_show_string(10, 16 * 4, "Ro");
                ips114_show_string(10, 16 * 5, "D");

                ips114_show_string(0, 16 * 6, "K4");
                observe_drawn = 1;
            }

            ips114_show_float(40, 16 * 0, ADC_temp[0], 3, 1);
            ips114_show_float(40, 16 * 1, ADC_temp[1], 3, 1);
            ips114_show_float(40, 16 * 2, ADC_temp[3], 3, 1);
            ips114_show_float(40, 16 * 3, ADC_temp[2], 3, 1);
            ips114_show_float(40, 16 * 4, imu660rc_roll, 4, 1);
            ips114_show_string(40, 16 * 5, "    ");
            break;
    }

    last_menu_state = menu_state;
    last_cursor_index = cursor_index;
}