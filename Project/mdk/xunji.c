#include "xunji.h"

static void ringL_start(void);
static void ringR_start(void);
static void ringL_task(void);
static void ringR_task(void);
static void ring_execute(void);
static void qqb_update(void);
static void ring_detect_update(void);
static int16 sudu_juece(int16 MaxSpeed, int16 MinSpeed);
static uint8 ring_plan_total_count(void);
static uint8 ring_plan_get_action(uint8 index);
static uint8 ring_plan_digit_get(uint8 index);

#define huan_zhi_dist 3000
#define huan_after_dist 10000
#define ring_detect_cooldown_count 750
#define ring_detect_inductor_threshold 120
#define ring_detect_roll_threshold 60
#define ring_detect_roll_reset_min -10
#define ring_detect_roll_reset_max 10
int8 element = 0;
int8 ring_flag_task = 0;
static int8 ring_flag_execute = huan_zhi;
static uint8 ring_detect_count = 0;
static uint8 ring_completed_laps = 0;
static uint8 ring_current_index = 0;
static uint16 ring_detect_cooldown = 0;
static uint8 ring_detect_latch = 0;
static uint8 ring_stop_after_cooldown = 0;
static uint8 ring_stop_on_transition = 0;
static uint8 ring_pid_stop_active = 0;
static float distance_ring = 0;
static float huan_after_sum = 0;
static uint8 huan_after_flag = 0;
static float last_absp = 0;
int16 speed_straight = 250;
int16 speed_ringR = 250;
uint16 qqb_low_speed_delay = 150;//跷跷板低速时间 150x2=300ms
int16 qqb_low_speed_target = 50;//跷跷板低速速度 50
uint8 ring_plan_laps = 2;
uint8 ring_plan_seq_len = 3;
uint8 ring_plan_seq_pos = 1;
uint32 ring_plan_seq_code = 20;
uint8 ring_plan_stop_action = 2;
static uint8 qqb_flag = 0;
static uint16 qqb_low_speed_count = 0;
static uint8 qqb_detect_latch = 0;



void xunji(void)
{
    qqb_update();
    ring_detect_update();

    if (start_ramp_flag == 0)
        return;

    if (L + LM + RM + R < 10)//出界保护
    { 
        element = 9;
    }


    switch (element)
    {
    case 0:
        if (qqb_flag)
        {
            speed_target = qqb_low_speed_target;
        }
        else
        {
            speed_target = sudu_juece(speed_straight, 220);
        }
        left_target = speed_target + correct_L;
        right_target = speed_target - correct_L;
        speed_loop_LR(left_target, right_target);
        set_pwm_motor_R(out_R+speed_target*6.0f);
        set_pwm_motor_L(out_L+speed_target*6.0f);
        break;

    case 1:
        if (!ring_pid_stop_active)
            ringL_task();
        if (start_ramp_flag == 1)
            ring_execute();
        break;

    case 2:
        if (!ring_pid_stop_active)
            ringR_task();
        if (start_ramp_flag == 1)
            ring_execute();
        break;

    case 9:
        start_ramp_flag = 0;
        break;

    default:
        element = 0;
        break;
    }
}

static int16 sudu_juece(int16 MaxSpeed, int16 MinSpeed)
{
    float k, k1, k2, k3;

    k1 = deviation;
    if (k1 < 0) k1 = -k1;

    k2 = (LM + RM) - 6.0f;
    if (k2 < 0) k2 = 0;
    k2 = k2 / 100.0f;
    if (k2 > 1.0f) k2 = 1.0f;

    k3 = k1 - last_absp;
    last_absp = k1;
    k3 = k3 / 20.0f;
    if (k3 > 1.0f) k3 = 1.0f;

    k1 = k1 / 100.0f;
    if (k1 > 1.0f) k1 = 1.0f;

    k = k1 * 1.0f + k2 * 1.0f + k3 * 1.0f;

    if (k > 1.0f) k = 1.0f;
    if (k < 0.0f) k = 0.0f;

    return MaxSpeed - (int16)((MaxSpeed - MinSpeed) * k);
}

static void qqb_update(void)
{
    uint8 qqb_detected;

    qqb_detected = (L + LM + RM + R < 55 && imu660rc_roll > 10 && imu660rc_roll < 20);

    if (qqb_detected && !qqb_detect_latch)
    {
        xunji_debug_event = 4;
    }

    qqb_detect_latch = qqb_detected;

    if (qqb_detected)
    {
        qqb_low_speed_count = qqb_low_speed_delay;
    }
    else if (qqb_low_speed_count > 0)
    {
        qqb_low_speed_count--;
    }

    qqb_flag = (qqb_detected || qqb_low_speed_count > 0);
}



static uint8 ring_plan_total_count(void)
{
    uint8 total;

    total = ring_plan_seq_len;
    if (total < 1) total = 1;
    if (total > 8) total = 8;
    return total;
}

static uint8 ring_plan_digit_get(uint8 index)
{
    uint8 i;
    uint8 len;
    uint32 divisor;
    uint8 digit;

    len = ring_plan_total_count();
    if (index < 1 || index > len)
        return 0;

    divisor = 1;
    for (i = index; i < len; i++)
    {
        divisor *= 10;
    }

    digit = (uint8)((ring_plan_seq_code / divisor) % 10);
    if (digit > 2)
        digit = 0;

    return digit;
}

static uint8 ring_plan_get_action(uint8 index)
{
    return ring_plan_digit_get(index);
}

static void ring_detect_update(void)
{
    uint8 action;
    uint8 total;
    uint8 laps;
    uint8 ring_detected;

    ring_detected = (L + R > ring_detect_inductor_threshold || imu660rc_roll > ring_detect_roll_threshold);

    if (L + R <= ring_detect_inductor_threshold &&
        imu660rc_roll >= ring_detect_roll_reset_min &&
        imu660rc_roll <= ring_detect_roll_reset_max)
    {
        ring_detect_latch = 0;
    }

    if (ring_detect_cooldown > 0)
    {
        ring_detect_cooldown--;
        if (ring_detect_cooldown == 0 && ring_stop_after_cooldown)
        {
            ring_stop_after_cooldown = 0;
            start_ramp_flag = 0;
        }
        return;
    }

    if (ring_detect_latch || element != 0 || ring_flag_task != 0)
    {
        return;
    }

    laps = ring_plan_laps;
    if (laps < 1) laps = 1;

    if (ring_completed_laps > laps)
    {
        return;
    }

    if (ring_detected)
    {
        ring_detect_latch = 1;
        total = ring_plan_total_count();
        ring_detect_count++;
        if (ring_detect_count > total)
            ring_detect_count = 1;

        ring_current_index = ring_detect_count;
        action = ring_plan_get_action(ring_current_index);
        xunji_debug_ring_index = action;
        ring_detect_cooldown = ring_detect_cooldown_count;
        if (ring_completed_laps >= laps && action == ring_plan_stop_action)
        {
            if (action == 0)
                ring_stop_after_cooldown = 1;
            else
                ring_stop_on_transition = 1;
        }
        if (action == 1)
        {
            ringL_start();
        }
        else if (action == 2)
        {
            ringR_start();
        }
        else
        {
            xunji_debug_event = 8;
        }

        if (ring_detect_count >= total)
        {
            ring_detect_count = 0;
            ring_completed_laps++;
        }
    }
}

static void ringL_start(void)
{
    element = 1;
    ring_flag_task = huan_zhi;
    ring_flag_execute = huan_zhi;
    distance_ring = 0;
    xunji_debug_L = L;
    xunji_debug_LM = LM;
    xunji_debug_RM = RM;
    xunji_debug_R = R;
    xunji_debug_event = 1;
}

static void ringR_start(void)
{
    element = 2;
    ring_flag_task = huan_zhi;
    ring_flag_execute = huan_zhi;
    distance_ring = 0;
    xunji_debug_L = L;
    xunji_debug_LM = LM;
    xunji_debug_RM = RM;
    xunji_debug_R = R;
    xunji_debug_event = 6;
}

static void ringL_task(void)
{
    switch (ring_flag_task)
    {
    case huan_zhi:
        distance_ring += speed_avl;
        if (distance_ring > huan_zhi_dist)
        {
            ring_flag_task = huan_ru;
            ring_flag_execute = huan_ru;
            distance_ring = 0;
        }
        break;

    case huan_ru:
        distance_ring += speed_avl;
        if (distance_ring > 30000)
        {
            ring_flag_task = huan_chu;
            ring_flag_execute = huan_chu;
            distance_ring = 0;
            if (ring_stop_on_transition)
            {
                ring_stop_on_transition = 0;
                ring_pid_stop_active = 1;
            }
            gpio_set_level(IO_P52, 0);
        }
        break;

    case huan_chu:
        distance_ring += speed_avl;
        if (distance_ring > 25000)
        {
            distance_ring = 0;
            ring_flag_task = 0;
            huan_after_flag = 1;
            huan_after_sum = 0;
            ring_flag_execute = huan_zhi;
            element = 0;
            xunji_debug_event = 2;




        }
        break;

    default:
        break;
    }
}

static void ringR_task(void)
{
    switch (ring_flag_task)
    {
    case huan_zhi:
        distance_ring += speed_avl;
        if (distance_ring > huan_zhi_dist)
        {
            ring_flag_task = huan_ru;
            ring_flag_execute = huan_ru;
            distance_ring = 0;
        }
        break;

    case huan_ru:
        distance_ring += speed_avl;
        if (distance_ring > 15000)
        {
            ring_flag_task = huan_chu;
            ring_flag_execute = huan_chu;
            distance_ring = 0;
            if (ring_stop_on_transition)
            {
                ring_stop_on_transition = 0;
                ring_pid_stop_active = 1;
            }
           
        }
        break;

    case huan_chu:
        distance_ring += speed_avl;
        if (distance_ring > 30000)
        {
            distance_ring = 0;
            ring_flag_task = 0;
            huan_after_flag = 1;
            huan_after_sum = 0;
            ring_flag_execute = huan_zhi;
            element = 0;
            xunji_debug_event = 2;



        }
        break;

    default:
        break;
    }
}

static void ring_execute(void)
{
    if (ring_pid_stop_active)
    {
        speed_target = 0;
        left_target = 0;
        right_target = 0;
        speed_loop_LR(left_target, right_target);
        set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);
        fuya_set_duty(0);
        return;
    }

    switch (ring_flag_execute)
    {
    case huan_zhi:
        speed_target = qqb_flag ? qqb_low_speed_target : speed_straight;
        left_target = speed_target;
        right_target = speed_target;
        speed_loop_LR(left_target, right_target);
        break;

    case huan_ru:
    case huan_chu:
        speed_target = qqb_flag ? qqb_low_speed_target : speed_ringR;
        left_target = speed_target + correct_L;
        right_target = speed_target - correct_L;
        speed_loop_LR(left_target, right_target);
        break;

    default:
        return;
    }

        set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);
    if (L + LM + RM + R < 10)
        start_ramp_flag = 0;
}

void xunji_state_reset(void)
{
	element = 0;
	ring_flag_task = 0;
	ring_flag_execute = huan_zhi;
	ring_detect_count = 0;
	ring_completed_laps = 0;
	ring_current_index = 0;
	ring_detect_cooldown = 0;
	ring_detect_latch = 0;
	ring_stop_after_cooldown = 0;
	ring_stop_on_transition = 0;
	ring_pid_stop_active = 0;
	distance_ring = 0;
	huan_after_sum = 0;
	huan_after_flag = 0;
	last_absp = 0;
	qqb_flag = 0;
	qqb_low_speed_count = 0;
	qqb_detect_latch = 0;
	xunji_debug_event = 0;
	xunji_debug_ring_index = 0;
	gpio_set_level(IO_P52, 1);
}
