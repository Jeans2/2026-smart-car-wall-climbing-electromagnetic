#include "xunji.h"

int16 turnspeed = 0 ;
int16 correct_speed_L,correct_speed_R;
float abs_angle = 0;


static uint16 huan_cooldown = 0; // 冷却计数器
// 状态变量初始化
RoundState cur_state = ROUND_NONE;
int8 huan_flag = 0;
int8 direct_l = 0, direct_r = 0;


// 环岛变量初始化
void huan_init(void) {
    cur_state = ROUND_NONE;
    huan_flag = 0;
    direct_l = 0;
    direct_r = 0;
    angle = 0;
}





// 1. 环岛检测逻辑
void huan_check() 
{
    if (huan_flag == 1) return; // 已在环岛内
    if (huan_cooldown > 0) 
    { 
				huan_cooldown--; 
				return; 
		} // 冷却中

    // 左环触发判断
		if (ADC_temp[0]>900 &&(ADC_temp[0]+ADC_temp[1])>1400)
//    if (L > R && LM > RM * 6 && (L + R + LM + RM) > HUAN_TRIGGER_VAL) 
		{
        huan_flag = 1;
        direct_l = 1; direct_r = 0;
        angle = 0; // 核心：触发瞬间角度清零
        cur_state = ROUND_IN;     	
    }
    // 右环触发判断
		else if (ADC_temp[2]>900)
//    else if (R > L && RM > LM * 6 && (L + R + LM + RM) > HUAN_TRIGGER_VAL) 
		{
        huan_flag = 1;
        direct_l = 0; direct_r = 1;
        angle = 0;
        cur_state = ROUND_IN;
    }
}

// 2. 环岛状态机切换（基于角度绝对值）
void benhuan() {
    if (huan_flag == 0) 
		{
		  return;
		}	
		

     abs_angle = fabs(angle); // 统一绝对值，不管左旋右旋

    switch (cur_state) 
		{
        case ROUND_IN:
            if (abs_angle >= HUAN_ANGLE_IN) {
                cur_state = ROUND_KEEP; // 角度够了，立刻变更为环内循迹
            }
            break;

        case ROUND_KEEP:
            if (abs_angle >= HUAN_ANGLE_OUT) {
                cur_state = ROUND_PREOUT; // 准备出环
            }
            break;

        case ROUND_PREOUT:
            // 出环判定：中间两个电感重新找回直道特征，或侧边电感离开圆弧
            if (abs(LM - RM) < 40) { 
                cur_state = ROUND_EXIT;
            }
            break;

        case ROUND_EXIT:
            huan_init(); // 重置所有参数
            huan_cooldown = 500; // 设置冷却，防止出环瞬间误触发
            break;
            
        default: break;
    }
}

// 3. 循迹速度执行
void xunji() 
{
     turnspeed = (int16)out(); // 获取正常PID输出
 
    if (huan_flag == 1) 
		{
        switch (cur_state) {
            case ROUND_IN: // 入环：强制差速（不管传感器）
                if (direct_l == 1) { // 左转：右轮快，左轮慢
                    correct_speed_L = speed_target - 40;
                    correct_speed_R = speed_target + 40;
                } else { // 右转：左轮快，右轮慢
                    correct_speed_L = speed_target + 15;
                    correct_speed_R = speed_target - 15;
                }
                break;

            case ROUND_KEEP: // 环内：高增益循迹
                correct_speed_L = speed_target - turnspeed * 1.2f-10;
                correct_speed_R = speed_target + turnspeed * 1.2f+10;
                break;

            case ROUND_PREOUT: // 出环：低增益 + 强制回正力
                // 假设出环时给一个向外的固定补偿 (5)
                if (direct_l == 1) {
                    correct_speed_L = speed_target - turnspeed * 0.4f + 5;
                    correct_speed_R = speed_target + turnspeed * 0.4f - 5;
                } else {
                    correct_speed_L = speed_target - turnspeed * 0.4f - 5;
                    correct_speed_R = speed_target + turnspeed * 0.4f + 5;
                }
                break;

            default: // 兜底
                correct_speed_L = speed_target - turnspeed;
                correct_speed_R = speed_target + turnspeed;
                break;
        }
    } else {
        // 正常行驶逻辑
        correct_speed_L = speed_target - turnspeed;
        correct_speed_R = speed_target + turnspeed;
    }

    // 执行电机输出
    speed_loop_LR(correct_speed_L, correct_speed_R);
    set_pwm_motor_R(out_R);
    set_pwm_motor_L(out_L);
}






//  状态显示函数
void display_huan_state()
{
    switch (cur_state) 
		{
        case ROUND_NONE:   ips114_show_string(0,16*5, "N   "); break; // 正常行驶
        case ROUND_IN:     ips114_show_string(0,16*5, "I      N"); break; // 正在打角
        case ROUND_KEEP:   ips114_show_string(0,16*5, "KEEP   "); break; // 环内循迹
        case ROUND_PREOUT: ips114_show_string(0,16*5, "PRE_OUT"); break; // 准备出环
        case ROUND_EXIT:   ips114_show_string(0,16*5, "EXIT   "); break; // 正在退出
        default:           ips114_show_string(0,16*5, "ERROR  "); break;
    }
}



