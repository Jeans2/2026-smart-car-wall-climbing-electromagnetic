#include "adc.h"

//1-电磁
uint16 ADC_original[4][ADC_Sample_Num]={0};
uint16 max[4]={0},min[4]={0};
float ADC_temp[4] = {0};
float L=0,LM=0,RM=0,R=0;






//adc初始化
void my_adc_init(void)
{
		adc_init(ADC_L_CH ,ADC_10BIT);//L
		adc_init(ADC_LM_CH ,ADC_10BIT);//LM
		adc_init(ADC_R_CH ,ADC_10BIT);//R
		adc_init(ADC_RM_CH ,ADC_10BIT);//RM
		adc_init(ADC_V ,ADC_10BIT);
}	
	
//电磁采样
uint16 adc_sample(adc_channel_enum ch)//共采样11次，去掉2个极值，剩余9个求平均
{
	uint16 temp,sum=0;
	uint16 max,min;
	uint8 i;
//	
	temp = adc_convert(ch);
	max = temp;
	min = temp;
	sum += temp;

	for(i=0;i<10;i++)
	{
		temp = adc_convert(ch);
		if(max<temp){max = temp;}
		if(min>temp){min = temp;}
		sum += temp;
	}
	
	temp = (sum-max-min) / (11-2);
	
	return temp;
}	
	
uint16 adc_sample_b(adc_channel_enum ch)//8次复合采样
{
	uint16 temp,sum=0;
	uint16 max,min;
	uint8 i;
	
	temp = adc_convert(ch);
	max = temp;
	min = temp;
	sum += temp;
	
	for(i=0;i<7;i++)
	{
		temp = adc_convert(ch);
		if(max<temp){max = temp;}
		if(min>temp){min = temp;}
		sum += temp;	
	}
	
	temp = (sum-max-min) / (8-2);
	
	return temp;	
}

uint16 adc_sample_a(adc_channel_enum ch)//7次排序取中值
{
	uint8 i;
	uint16 arr[7];
	
	for(i=0;i<ADC_Sample_Num;i++)
	{
		arr[i] = adc_convert(ch);
	}
	
	arr[0] = sort_seven(arr);	
	return arr[0];
}

uint16 adc_mid_sample(adc_channel_enum ch)//三次取中值
{
    uint16 i,j,k,tmp;
    //sample 3 times
    i = adc_convert(ch);
    j = adc_convert(ch);
    k = adc_convert(ch);
    //  select mid value
    if(i > j){
        tmp = i;
        i = j;
        j = tmp;
    }
    if(k > j){
        tmp = j;
    }
    else if(k > i){
        tmp = k;
    }
    else{
        tmp = i;
    }
    return tmp;
}

void siai_adc_all_sample(void)//存储原始采样数据
{
	ADC_temp[0] = adc_sample(ADC_L_CH );
	ADC_temp[1] = adc_sample(ADC_LM_CH );
	ADC_temp[2] = adc_sample(ADC_R_CH );
	ADC_temp[3] = adc_sample(ADC_RM_CH);
}

//归一化
void adc_normalizing(void)
{
	
 
	L  = 100*(ADC_temp[0]-0)/(1000-0);//600为待定值，后可取环岛与直道重叠部分所采集到的值
  LM = 100*(ADC_temp[1]-0)/(1000-0);
	RM = 100*(ADC_temp[3]-0)/(1000-0);
	R  = 100*(ADC_temp[2]-0)/(1000-0);
	
	//输入限幅
	if(L >100){L =100;}
	if(LM>100){LM=100;}
	if(RM>100){RM=100;}
	if(R >100){R =100;}
}


float deviation;
struct ADC adc_set_differ = {1,1,1};  // 1.3 1 0.8
float add = 0,sub = 0;
void adc_differ()     //差比和差
{
	sub = adc_set_differ.A*(L - R)+adc_set_differ.B*(LM - RM);
	add = adc_set_differ.A*(L + R)+adc_set_differ.C * abs(LM - RM);
	deviation = (sub*100) / (add+1);
	deviation = -deviation;
	
}


