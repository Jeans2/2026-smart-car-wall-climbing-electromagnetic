#ifndef __ENCODER_H
#define __ENCODER_H

#include "bsp_system.h"


#define ENCODER_DIR_1                 	(TIM0_ENCOEDER)                         
#define ENCODER_DIR_DIR_1              	(IO_P35)            				 
#define ENCODER_DIR_PULSE_1            	(TIM0_ENCOEDER_P34)            		

#define ENCODER_DIR_2                 	(TIM3_ENCOEDER)                         
#define ENCODER_DIR_DIR_2           	(IO_P53)             					
#define ENCODER_DIR_PULSE_2       		(TIM3_ENCOEDER_P04)            			




extern int16 speed_L,speed_R,speed_avl;




void encoder_init(void);
void encoder_update(void);





#endif