#include "encoder.h"

SlidingAverageFilter filter_Left;
SlidingAverageFilter filter_Right;
float speed_L=0,speed_R=0,speed_avl=0;
float speed_L_next=0,speed_R_next=0,speed_avl_next;
int16 lastspeed_L=0,lastspeed_R=0;
int32 distance_text=0;


void encoder_unit(void)
{
		encoder_dir_init(ENCODER_DIR_1, ENCODER_DIR_DIR_1, ENCODER_DIR_PULSE_1);   	// 初始化编码器模块与引脚 带方向增量编码器模式
    encoder_dir_init(ENCODER_DIR_2, ENCODER_DIR_DIR_2, ENCODER_DIR_PULSE_2);    // 初始化编码器模块与引脚 带方向增量编码器模式
		
		initSlidingAverage(&filter_Left,6);  // 初始化滤波器
	  initSlidingAverage(&filter_Right,6);
}


void encoder_update(void)
{
	//0-获取速度&清零
	speed_R = (0.85f*encoder_get_count(ENCODER_DIR_1));
	speed_L = (0.85f*encoder_get_count(ENCODER_DIR_2));
	
	encoder_clear_count(ENCODER_DIR_2);
	encoder_clear_count(ENCODER_DIR_1);
	//1-一阶低通滤波
	speed_L=(0.9*speed_L + 0.1*lastspeed_L);
	speed_R=(0.9*speed_R + 0.1*lastspeed_R);
	lastspeed_L = speed_L;
	lastspeed_R = speed_R;
	 
	speed_avl = (speed_L + speed_R)/2;
	
	distance_text += (int32)speed_avl;
}
