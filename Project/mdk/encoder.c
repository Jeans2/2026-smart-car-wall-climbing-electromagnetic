#include "encoder.h"

#define encoder_lpf_old 8
#define encoder_lpf_new 2
#define encoder_lpf_den 10

static int16 tempL_pluse;
static int16 tempR_pluse;
static int16 speed_L_raw = 0, speed_R_raw = 0;
static int32 speed_L_lpf = 0, speed_R_lpf = 0;
int16 speed_L=0,speed_R=0,speed_avl=0;




void encoder_init(void)
{
		encoder_dir_init(ENCODER_DIR_1, ENCODER_DIR_DIR_1, ENCODER_DIR_PULSE_1);   
    encoder_dir_init(ENCODER_DIR_2, ENCODER_DIR_DIR_2, ENCODER_DIR_PULSE_2);   
		
}


void encoder_update(void)
{
	
			tempL_pluse = encoder_get_count(ENCODER_DIR_1);
			encoder_clear_count(ENCODER_DIR_1);
		  
			tempR_pluse = encoder_get_count(ENCODER_DIR_2);
			encoder_clear_count(ENCODER_DIR_2);
		
						
        speed_L_raw = -tempL_pluse;
        speed_R_raw = tempR_pluse;
        speed_L_lpf = (speed_L_lpf * encoder_lpf_old + (int32)speed_L_raw * encoder_lpf_new) / encoder_lpf_den;
        speed_R_lpf = (speed_R_lpf * encoder_lpf_old + (int32)speed_R_raw * encoder_lpf_new) / encoder_lpf_den;
        speed_L = (int16)speed_L_lpf;
        speed_R = (int16)speed_R_lpf;
	speed_avl = (speed_L + speed_R)/2;

}
