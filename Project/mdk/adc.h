#ifndef __ADC_H
#define __ADC_H

#include "bsp_system.h"

#define ADC_Sample_Num 7

#define ADC_L_CH   ADC_CH8_P00
#define ADC_LM_CH  ADC_CH9_P01
#define ADC_RM_CH  ADC_CH13_P05
#define ADC_R_CH   ADC_CH14_P06
#define ADC_V      ADC_CH2_P12

struct ADC {
	float A;
	float B;
	float C;
};

extern struct ADC adc_set_differ;
extern float deviation;
extern float ADC_temp[4];
extern float L, LM, RM, R;

void my_adc_init(void);
void siai_adc_all_sample(void);
void adc_normalizing(void);
void adc_differ(void);

#endif
