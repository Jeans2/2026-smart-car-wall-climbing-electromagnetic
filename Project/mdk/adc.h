#ifndef __ADC_H
#define __ADC_H

#include "bsp_system.h"
#define ADC_Sample_Num 7


#define ADC_L_CH    ADC_CH8_P00//L1
#define ADC_LM_CH   ADC_CH9_P01//L2
#define ADC_RM_CH    ADC_CH13_P05//L3
#define ADC_R_CH   ADC_CH14_P06//L4

#define ADC_V       ADC_CH2_P12

//ADC
extern struct ADC{
	
	float A;
	float B;
	float C;
	
};

extern struct ADC adc_set_differ;
extern float deviation;
extern float ADC_temp[4];//存储4个adc值
extern float L,LM,RM,R;//归一化后传感器值

extern float adc_v_1,adc_v_2,adc_v_3,adc_v_4;
extern float adc_start;
//算法
extern uint16 ADC_original[4][ADC_Sample_Num];//原始采样数据
void my_adc_init(void);//初始化ADC

//对电磁信号的采样
uint16 adc_sample(adc_channel_enum ch);//共采样11次，去掉2个极值，剩余9个求平均
uint16 adc_mid_sample(adc_channel_enum ch);//三次取中值
uint16 adc_sample_a(adc_channel_enum ch);//7次排序取中值
uint16 adc_sample_b(adc_channel_enum ch);//8次符复合采样
void siai_adc_all_sample(void);//存储原始采样数据
void adc_normalizing(void);//归一化
void adc_differ(void); //差比和差


#endif