#ifndef __ENCODER_H
#define __ENCODER_H

#include "bsp_system.h"


#define ENCODER_DIR_1                 	(TIM0_ENCOEDER)                         // 正交编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER1
#define ENCODER_DIR_DIR_1              	(IO_P35)            				 	// DIR 对应的引脚
#define ENCODER_DIR_PULSE_1            	(TIM0_ENCOEDER_P34)            			// PULSE 对应的引脚

#define ENCODER_DIR_2                 	(TIM3_ENCOEDER)                         // 带方向编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER2
#define ENCODER_DIR_DIR_2           	(IO_P53)             					// DIR 对应的引脚
#define ENCODER_DIR_PULSE_2       		(TIM3_ENCOEDER_P04)            			// PULSE 对应的引脚

extern SlidingAverageFilter filter_Left;
extern SlidingAverageFilter filter_Right;
extern float speed_L,speed_R,speed_avl;
extern float speed_L_next,speed_R_next,speed_avl_next;
extern int32 distance_text;






void encoder_unit(void);
void encoder_update(void);





#endif