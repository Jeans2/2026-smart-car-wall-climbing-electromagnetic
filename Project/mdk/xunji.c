#include "xunji.h"

static void ringR_task(void);
static void ringR_execute(void);

int8 element = 0;
int8 ringR_flag_task = 0;
static int8 ringR_flag_execute = 1;
static uint8 ring_flag_ing = 0;
static float distance_ringR = 0;
int16 speed_straight =180;
int16 speed_ringR = 180;
static uint8 ring_data_sent = 0;
static float yaw_start = 0;
static float yaw_last  = 0;
static float yaw_sum   = 0;
float ring_entry_dist = 0;

#define QQB_slow_dist_target  20000.0f
#define QQB_speed               100
#define QQB_unlock_dist_target 90000.0f
#define QQB_unlock_roll_angle   150.0f
#define ring_roll_max_angle     40.0f

static float QQB_slow_dist = 0;
static uint8 QQB_detect_enable = 0;
static float QQB_unlock_dist = 0;
static uint8 QQB_roll_latch = 1;

void xunji(void)
{
  

    if (imu660rc_roll < ring_roll_max_angle && imu660rc_roll > 0)
        QQB_roll_latch = 0;

    if (element == 0 && QQB_detect_enable == 0)
    {
        if (QQB_unlock_dist > 0)
        {
            QQB_unlock_dist += speed_avl;
            if (QQB_unlock_dist >= QQB_unlock_dist_target)
            {
                QQB_unlock_dist = 0;
                QQB_slow_dist = 0;
                element = 2;
                return;
            }
        }
        else if (QQB_roll_latch == 0 && imu660rc_roll > QQB_unlock_roll_angle)
        {
            QQB_unlock_dist = 1;
            QQB_roll_latch = 1;
            xunji_debug_roll = imu660rc_roll;
            xunji_debug_event = 5;
            return;
        }
    }
if (element == 0&& L + R > 115 && LM+RM<30 && ring_flag_ing == 0)//环岛判断
    {
			dl1a_get_distance();
			if(dl1a_distance_mm >4000&&dl1a_distance_mm <8500)
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
            xunji_debug_L = L;
            xunji_debug_LM = LM;
            xunji_debug_RM = RM;
            xunji_debug_R = R;
            xunji_debug_roll = imu660rc_roll;
            xunji_debug_distance = dl1a_distance_mm;
            xunji_debug_event = 2;
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

        QQB_slow_dist += speed_avl;
        if (QQB_slow_dist >= QQB_slow_dist_target)//减速一段距离后回归循迹
        {
            QQB_slow_dist = 0;
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
