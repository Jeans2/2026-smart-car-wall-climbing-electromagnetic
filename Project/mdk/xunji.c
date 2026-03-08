#include "xunji.h"

int16 turnspeed = 0 ;
int16 correct_speed_L,correct_speed_R;


void xunji()
{	
	
			turnspeed= (int16) out();
			// 2. 根据环岛状态选择控制模式
			if(Angle_huan_flag1 == 1&& direct_l == 1)  // 检测到入环打角标志 左环
      {
        // 环岛内可以用稍慢的速度     
         correct_speed_L = speed_target + 30 ;
         correct_speed_R = speed_target ;
      }
			else if(Angle_huan_flag1 == 1&& direct_r == 1)  // 检测到入环打角标志 右环
      {
        // 环岛内可以用稍慢的速度     
         correct_speed_L = speed_target ;
         correct_speed_R = speed_target + 30;
      }
      else //未检测到环 正常行驶
      {
            
        correct_speed_L = speed_target - turnspeed;
        correct_speed_R = speed_target + turnspeed;
      }

//	if(correct_speed_L>100){correct_speed_L=55;}
//	if(correct_speed_L<0){correct_speed_L=10;}
//	if(correct_speed_R>100){correct_speed_L=55;}
//	if(correct_speed_R<0){correct_speed_L=10;}
			speed_loop_LR(correct_speed_L,correct_speed_R);
			set_pwm_motor_R(out_R);
			set_pwm_motor_L(out_L);
		
		
}



// 环岛状态变量
int8 huan_flag=0;           // 环岛触发标志
int8 direct_l = 0;					// 左环方向标志
int8 direct_r = 0;					// 右环方向标志
int8 Angle_huan_flag1 = 0;	// 入环打角标志
int8 Angle_huan_flag2 = 0;		// 环内循迹标志
int8 Angle_huanout_flag2 = 0;	 // 出环直行标志
float straght = 0;						// 出环直行位移
// 防抖计数器
static uint8 ring_trigger_cnt = 0;

// 环岛变量初始化
void huan_init(void)
{
    huan_flag = 0;
    direct_l = 0;
    direct_r = 0;
    angle = 0;
    Angle_huan_flag1 = 0;
    Angle_huan_flag2 = 0;
    Angle_huanout_flag2 = 0;
    straght = 0;
    ring_trigger_cnt = 0;
}



void huan_check()//环岛检测
{
		// 已经在环岛中就不重复检测
    if(huan_flag == 1) 
    {
        ring_trigger_cnt = 0;
        return;
    }
		// 左环岛触发判断：左内侧突然增大，且明显大于右内侧
		if(L > R && LM > RM * 6 && R+L+LM+RM>220)
		{
				ring_trigger_cnt++;
			if(ring_trigger_cnt >=3)// 连续触发3次，防抖
			{
					  huan_flag = 1;
            direct_l = 1;
            direct_r = 0;
            angle = 0;  // 清零角度积分
            ring_trigger_cnt = 0;
			
			
			}
   	}
		 else if(R > L && LM < RM * 6 && R+L+LM+RM>220)
    {
        ring_trigger_cnt++;
        if(ring_trigger_cnt >= 3)
        {
            huan_flag = 1;
            direct_l = 0;
            direct_r = 1;
            angle = 0;
            ring_trigger_cnt = 0;                        
        }
    }
	
}

void benhuan()
{
			// 只有在环岛标志有效时才执行状态机
    if(huan_flag == 0) return;
	
		// 左环处理
		if(direct_l == 1 )
		{
				// 状态1: 入环打角阶段 (0°到 -40°)
			if(angle > -HUAN_ANGLE_IN)
        {
            Angle_huan_flag1 = 1;
            Angle_huan_flag2 = 0;
            Angle_huanout_flag2 = 0;
        }
			 // 状态2: 环内循迹阶段 (-40°到 -300°)
        else if(angle <= -HUAN_ANGLE_IN && angle > -HUAN_ANGLE_OUT)
        {
            Angle_huan_flag1 = 0;
            Angle_huan_flag2 = 1;
            Angle_huanout_flag2 = 0;
        }
			// 状态3: 准备出环阶段 (≤ -300°)
        else if(angle <= -HUAN_ANGLE_OUT)
        {
            Angle_huan_flag1 = 0;
            Angle_huan_flag2 = 0;
            Angle_huanout_flag2 = 1;
        }
			// 出环直行计数
        if(Angle_huanout_flag2 == 1)
        {
            if(L < HUAN_TRIGGER_L && L < R* HUAN_RATIO)
            {
                // 环岛结束，清零所有标志
                Angle_huanout_flag2 = 0;
                huan_flag = 0;
                angle = 0;
               
            }
        }
		
		}
		
		// 右环处理
    if(direct_r == 1)
    {
        // 状态1: 入环打角阶段 (0°到 40°)
        if(angle < HUAN_ANGLE_IN)
        {
            Angle_huan_flag1 = 1;
            Angle_huan_flag2 = 0;
            Angle_huanout_flag2 = 0;
        }
        // 状态2: 环内循迹阶段 (40°到 300°)
        else if(angle >= HUAN_ANGLE_IN && angle < HUAN_ANGLE_OUT)
        {
            Angle_huan_flag1 = 0;
            Angle_huan_flag2 = 1;
            Angle_huanout_flag2 = 0;
        }
        // 状态3: 准备出环阶段 (≥ 300°)
        else if(angle >= HUAN_ANGLE_OUT)
        {
            Angle_huan_flag1 = 0;
            Angle_huan_flag2 = 0;
            Angle_huanout_flag2 = 1;
        }
        
        // 出环直行计数
        if(Angle_huanout_flag2 == 1)
        {
            if(R < HUAN_TRIGGER_R && R < L * HUAN_RATIO)
            {
                Angle_huanout_flag2 = 0;
                huan_flag = 0;
                angle = 0;
              
            }
        }
    }



}