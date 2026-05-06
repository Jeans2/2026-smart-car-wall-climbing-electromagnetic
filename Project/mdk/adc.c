#include "adc.h"

static uint16 ADC_original[4][ADC_Sample_Num] = {0};
float ADC_temp[4] = {0};
float L = 0, LM = 0, RM = 0, R = 0;

void my_adc_init(void)
{
	adc_init(ADC_L_CH,  ADC_10BIT);
	adc_init(ADC_LM_CH, ADC_10BIT);
	adc_init(ADC_R_CH,  ADC_10BIT);
	adc_init(ADC_RM_CH, ADC_10BIT);
}

static uint16 adc_sample(adc_channel_enum ch)
{
	uint16 temp, sum = 0, max_v, min_v;
	uint8 i;

	temp = adc_convert(ch);
	max_v = temp;
	min_v = temp;
	sum += temp;

	for (i = 0; i < 10; i++)
	{
		temp = adc_convert(ch);
		if (max_v < temp) max_v = temp;
		if (min_v > temp) min_v = temp;
		sum += temp;
	}

	return (sum - max_v - min_v) / 9;
}

void siai_adc_all_sample(void)
{
	ADC_temp[0] = adc_sample(ADC_L_CH);
	ADC_temp[1] = adc_sample(ADC_LM_CH);
	ADC_temp[2] = adc_sample(ADC_R_CH);
	ADC_temp[3] = adc_sample(ADC_RM_CH);
}

void adc_normalizing(void)
{
	L  = 100.0f * (ADC_temp[0] - 0) / (1000 - 0);
	LM = 100.0f * (ADC_temp[1] - 0) / (1000 - 0);
	RM = 100.0f * (ADC_temp[3] - 0) / (1000 - 0);
	R  = 100.0f * (ADC_temp[2] - 0) / (1000 - 0);

	if (L  > 100) L  = 100;
	if (LM > 100) LM = 100;
	if (RM > 100) RM = 100;
	if (R  > 100) R  = 100;
}

float deviation;
struct ADC adc_set_differ = {1.0f, 2.3f, 1.2f};

void adc_differ(void)
{
	float sub, add;

	sub = adc_set_differ.A * (L - R) + adc_set_differ.B * (LM - RM);
	{
		float diff = LM - RM;
		if (diff < 0) diff = -diff;
		add = adc_set_differ.A * (L + R) + adc_set_differ.C * diff;
	}
	deviation = -(sub * 100.0f) / (add + 1);
}
