#ifndef __ENCODER_H
#define __ENCODER_H

#include "bsp_system.h"


#define ENCODER_DIR_1                 	(TIM0_ENCOEDER)                         // ������������Ӧʹ�õı������ӿ� ����ʹ��QTIMER1��ENCOEDER1
#define ENCODER_DIR_DIR_1              	(IO_P35)            				 	// DIR ��Ӧ������
#define ENCODER_DIR_PULSE_1            	(TIM0_ENCOEDER_P34)            			// PULSE ��Ӧ������

#define ENCODER_DIR_2                 	(TIM3_ENCOEDER)                         // �������������Ӧʹ�õı������ӿ� ����ʹ��QTIMER1��ENCOEDER2
#define ENCODER_DIR_DIR_2           	(IO_P53)             					// DIR ��Ӧ������
#define ENCODER_DIR_PULSE_2       		(TIM3_ENCOEDER_P04)            			// PULSE ��Ӧ������




extern int16 speed_L,speed_R,speed_avl;




void encoder_init(void);
void encoder_update(void);





#endif