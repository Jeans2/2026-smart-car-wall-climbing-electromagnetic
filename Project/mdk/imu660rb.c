#include "imu660rb.h"

float null_drift_z;	//零漂
float gyro_z;
float angle;
float avl_gyro_z;
float gyro_z_filtered;
float alpha = 0.9 ;

void gyro_get()      //角速度
{
	imu660rb_get_gyro();    	//获取陀螺仪数据
	gyro_z = imu660rb_gyro_z;
	gyro_z = imu660rb_gyro_transition(imu660rb_gyro_z);	
	avl_gyro_z = gyro_z - null_drift_z;
	gyro_z_filtered = alpha *gyro_z_filtered +(1-alpha)*avl_gyro_z;
}

void angle_get()     //角度获取
{
	

		angle+=0.002*avl_gyro_z;

	 
}

void angle_clear()   //角度清零
{
	angle = 0;
}

void null_drift_calculate()     //零漂采集
{
	static int16 cnt_null=0;
	static float temp=0;
	static int8 ret=0;
	
	//采集数据
//	
	
	temp += gyro_z;
		
	cnt_null++;
	if(ret=0 && cnt_null>=200)
	{
		null_drift_z = temp/200;
		ret = 1;
	}
	
}
