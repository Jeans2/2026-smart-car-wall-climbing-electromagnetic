#include "imu660rb.h"

float null_drift_z=0;	//零漂
float avl_gyro_z=0;   //测量
float gyro_z[2]={0}, next_gyro_z=0;			//(被测)Z轴角速度
float angle_ringR=0;
float z=0;


void gyro_get()				//角速度,角度获取
{	
	imu660rb_get_gyro();
	
	gyro_z[0] = 0.9*(-imu660rb_gyro_z)+0.1*gyro_z[1];
	gyro_z[1] = gyro_z[0];
	next_gyro_z = imu660ra_gyro_transition(gyro_z[0]);
	avl_gyro_z = next_gyro_z - null_drift_z;
	angle_get();
}

void angle_get()
{
	angle_ringR+=0.002*avl_gyro_z;

}

void angle_clear()
{
	angle_ringR=0;
}

int8 null_drift_calculate()
{
	static int16 cnt_null=0;
	static float temp=0;
	static int8 ret=0;
	
	//采集数据
//	
	
	temp += next_gyro_z;
		
	cnt_null++;
	if(cnt_null>=200)
	{
		null_drift_z = temp/200;
		ret = 1;
		angle_clear();
	}
	
	return ret;
}