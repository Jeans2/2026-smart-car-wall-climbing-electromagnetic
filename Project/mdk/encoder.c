#include "encoder.h"

SlidingAverageFilter filter_Left;
SlidingAverageFilter filter_Right;
int16 speed_L=0,speed_R=0,speed_avl=0;
float speed_L_next=0,speed_R_next=0,speed_avl_next;
int32 distance_text=0;




void encoder_init(void)
{
		encoder_dir_init(ENCODER_DIR_1, ENCODER_DIR_DIR_1, ENCODER_DIR_PULSE_1);   	// 初始化编码器模块与引脚 带方向增量编码器模式
    encoder_dir_init(ENCODER_DIR_2, ENCODER_DIR_DIR_2, ENCODER_DIR_PULSE_2);    // 初始化编码器模块与引脚 带方向增量编码器模式
		
		initSlidingAverage(&filter_Left,6);  // 初始化滤波器
	  initSlidingAverage(&filter_Right,6);
}


void encoder_update(void)
{
	
			tempL_pluse = encoder_get_count(ENCODER_DIR_1);
			encoder_clear_count(ENCODER_DIR_1);
		  
			tempR_pluse = encoder_get_count(ENCODER_DIR_2);
			encoder_clear_count(ENCODER_DIR_2);
		
						
		speed_L = -tempL_pluse;
		speed_R= tempR_pluse;
	speed_avl = (speed_L + speed_R)/2;

}
