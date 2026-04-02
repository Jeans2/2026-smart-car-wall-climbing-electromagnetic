#ifndef __ENCODER_H
#define __ENCODER_H

#include "bsp_system.h"


#define ENCODER_DIR_1                 	(TIM0_ENCOEDER)                         // 正交编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER1
#define ENCODER_DIR_DIR_1              	(IO_P35)            				 	// DIR 对应的引脚
#define ENCODER_DIR_PULSE_1            	(TIM0_ENCOEDER_P34)            			// PULSE 对应的引脚

#define ENCODER_DIR_2                 	(TIM3_ENCOEDER)                         // 带方向编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER2
#define ENCODER_DIR_DIR_2           	(IO_P53)             					// DIR 对应的引脚
#define ENCODER_DIR_PULSE_2       		(TIM3_ENCOEDER_P04)            			// PULSE 对应的引脚



#define encoder_pulses 1024 //编码器线数
#define wheel_d   4.0f  //轮子直径cm
#define time      0.002f//采样时间2ms
#define pi        3.1415926f
//extern SlidingAverageFilter filter_Left;
//extern SlidingAverageFilter filter_Right;
extern int16 speed_L,speed_R,speed_avl;
extern float speed_L_next,speed_R_next,speed_avl_next;
extern int32 distance_text;

static int16 tempL_pluse;
static int16 tempR_pluse;




void encoder_init(void);
void encoder_update(void);





#endif