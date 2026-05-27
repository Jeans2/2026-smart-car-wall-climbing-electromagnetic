#include "xunji.h"

static void ringR_task(void);
static void ringR_execute(void);

int8 element = 0;
int8 ringR_flag_task = 0;
static int8 ringR_flag_execute = 1;
static uint8 ring_flag_ing = 0;
static float distance_ringR = 0;
int16 speed_straight = 200;
int16 speed_ringR = 185;
static uint8 ring_data_sent = 0;
static float yaw_start = 0;
static float yaw_last  = 0;
static float yaw_sum   = 0;
float ring_entry_dist = 0;

#define QQB_stop_ticks       750
#define QQB_speed            140
#define QQB_detect_dist      200//到跷跷板距离

static uint8 QQB_after_ring_enable = 0;
static uint8 QQB_dist_sent = 0;
static float QQB_after_ring_dist = 0;
static uint8 QQB_lock = 1;
static uint16 QQB_stop_cnt = 0;

void xunji(void)
{
if (element == 0 &&                             //直线循迹时 判断跷跷板 距离大于0 小于350 
        QQB_lock == 0 &&       
        dl1a_distance_mm > 0 &&
        dl1a_distance_mm < QQB_detect_dist)
    {
        char buf[64];
        element = 2;
        QQB_lock = 1;
        QQB_stop_cnt = QQB_stop_ticks;
        out_L = 0;
        out_R = 0;
        err_speed_L_last = 0;
        err_speed_L = 0;
        err_speed_R_last = 0;
        err_speed_R = 0;
        direction_err1[0] = 0;
        direction_err1[1] = 0;
        direction_err1[2] = 0;
        correct_L = 0;
        sprintf(buf, "QQB ok,LR:%f,d:%u\r\n", L + R, dl1a_distance_mm);
        wireless_uart_send_string(buf);
    }

    if (element == 0 && L + R > 115 && LM+RM<30 && dl1a_distance_mm > 5000 && dl1a_distance_mm < 8500 && ring_flag_ing == 0)//环岛判断
    {              
            element = 1;
            ringR_flag_task = 1;
            ringR_flag_execute = 1;
            ring_flag_ing = 200;
            yaw_start = imu660rc_yaw;
            yaw_last  = imu660rc_yaw;
            yaw_sum   = 0;
            ring_entry_dist = 0;
        
    }

    if (L + LM + RM + R < 10)//出街保护
    {
        element = 9;
    }

    switch (element)
    {
    case 0:
        ring_data_sent = 0;
				speed_target = speed_straight;
        speed_loop_LR(speed_target + correct_L, speed_target - correct_L);
        set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);

        if (ring_flag_ing > 0)
            ring_flag_ing--;
        break;

    case 1:
        if (ring_data_sent == 0)
        {
            char buf[64];
            sprintf(buf, "L:%f,LM:%f,RM:%f,R:%f,distance:%u\r\n", L, LM, RM, R, dl1a_distance_mm);
            wireless_uart_send_string(buf);
            ring_data_sent = 1;
        }
        ringR_task();
        ringR_execute();
        break;

    case 2:
        speed_target = QQB_speed;
        speed_loop_LR(speed_target + correct_L, speed_target - correct_L);
        set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);

        if (QQB_stop_cnt > 0)//延时完回归循迹
            QQB_stop_cnt--;
        else
        {
            element = 0;
        }
        break;

    case 9:
        start_ramp_flag = 0;
        break;

    default:
        break;
    }
}

static void ringR_task(void)
{
    switch (ringR_flag_task)
    {
    case 1:
        {
            float dyaw = imu660rc_yaw - yaw_last;
            yaw_last = imu660rc_yaw;
            if (dyaw >  180) dyaw -= 360;
            if (dyaw < -180) dyaw += 360;
            yaw_sum += fabs(dyaw);
            if (yaw_sum > 270)
            {
                ringR_flag_task = 4;
                ringR_flag_execute = 4;
                distance_ringR = 0;
                yaw_sum = 0;
                gpio_set_level(IO_P52, 0);
            }
        }
        break;

    case 4:
        distance_ringR += speed_avl;
        if (distance_ringR > 45000)
        {
            distance_ringR = 0;
            ringR_flag_task = 0;
            ringR_flag_execute = 1;
            QQB_after_ring_enable = 1;
            QQB_after_ring_dist = 0;
            QQB_dist_sent = 0;
            QQB_lock = 0;
            element = 0;
        }
        break;

    default:
        break;
    }
}

static void ringR_execute(void)
{
    float target_L = 0;
    float target_R = 0;

    switch (ringR_flag_execute)
    {
    case 1:
        speed_target = speed_ringR;
        target_L = speed_target + correct_L;
        target_R = speed_target - correct_L;
        break;

    case 4:
        speed_target = speed_ringR;
        target_L = speed_target + correct_L;
        target_R = speed_target - correct_L;
        break;

    default:
        break;
    }

    if (ringR_flag_execute != 0)
    {
        speed_loop_LR(target_L, target_R);
        set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);
        if (L + LM + RM + R < 10)
            start_ramp_flag = 0;
    }
}
