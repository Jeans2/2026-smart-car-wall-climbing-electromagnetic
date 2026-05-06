#include "encoder.h"

static int16 tempL_pluse;
static int16 tempR_pluse;
static SlidingAverageFilter filter_Left;
static SlidingAverageFilter filter_Right;
int16 speed_L=0,speed_R=0,speed_avl=0;




void encoder_init(void)
{
		encoder_dir_init(ENCODER_DIR_1, ENCODER_DIR_DIR_1, ENCODER_DIR_PULSE_1);   
    encoder_dir_init(ENCODER_DIR_2, ENCODER_DIR_DIR_2, ENCODER_DIR_PULSE_2);   
		
		initSlidingAverage(&filter_Left,6);  
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
