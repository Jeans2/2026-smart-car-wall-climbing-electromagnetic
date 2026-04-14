#include "imu660rb.h"
#include "math.h"


float null_drift_z = 0;    // Z轴零漂
float null_drift_x = 0;    // X轴零漂

float gyro_z;
float gyro_x;
float angle_z = 0;
float angle_x = 0;
float avl_gyro_z;
float avl_gyro_x;

float gyro_z_filtered = 0;
float gyro_x_filtered = 0;
float alpha = 0.9;


float acc_z_filtered;
float acc_z;
float avl_acc_z;

float acc_y_filtered;
float acc_y;
float avl_acc_y;

float acc_x_filtered;
float acc_x;
float avl_acc_x;


float pitch;

// =========================================================================
// 函数名：gyro_init_calibration
// 功  能：开机静止状态下采集零漂 (必须在 main 函数的 while(1) 之前调用一次)
// =========================================================================
void gyro_init_calibration(void)
{
    int i;
    float temp_z = 0, temp_x = 0;
    
    // 连续采集 500 次求平均值
    for(i = 0; i < 500; i++)
    {
        imu660rb_get_gyro(); 
        temp_z += imu660rb_gyro_transition(imu660rb_gyro_z);
        temp_x += imu660rb_gyro_transition(imu660rb_gyro_x);
        system_delay_ms(1); 
    }
    null_drift_z = temp_z / 500.0f;
    null_drift_x = temp_x / 500.0f;
}

// =========================================================================
// 函数名：gyro_get_z / gyro_get_x
// 功  能：获取角速度 (扣除零漂 + 软件死区过滤)
// =========================================================================
void gyro_get_z(void)      
{
    imu660rb_get_gyro();        
    gyro_z = imu660rb_gyro_transition(imu660rb_gyro_z) - null_drift_z;
    
    // 软件死区：彻底杀死静态微小漂移
    if(gyro_z > -0.5f && gyro_z < 0.5f) gyro_z = 0;
    
    avl_gyro_z = gyro_z;
    gyro_z_filtered = alpha * gyro_z_filtered + (1 - alpha) * avl_gyro_z;
}

void gyro_get_x(void)      
{
    imu660rb_get_gyro();        
    gyro_x = imu660rb_gyro_transition(imu660rb_gyro_x) - null_drift_x;
    
    if(gyro_x > -0.5f && gyro_x < 0.5f) gyro_x = 0;
    
    avl_gyro_x = gyro_x;
    gyro_x_filtered = alpha * gyro_x_filtered + (1 - alpha) * avl_gyro_x;	
}

void angle_get(void)
{
    gyro_get_z();
    gyro_get_x();
		get_acc_z();
		get_acc_y();
	  get_acc_x();
    // 假设调用周期为 2ms
    angle_x += 0.002f * gyro_x_filtered;
    angle_z += 0.002f * gyro_z_filtered;
		
	pitch = atan2(acc_y_filtered,acc_z_filtered)*57.3;
	
}

void get_acc_z(void)
{	
		imu660rb_get_acc();
	 acc_z = imu660rb_acc_transition(-imu660rb_acc_z);

		avl_acc_z = acc_z;
    acc_z_filtered = alpha * acc_z_filtered + (1 - alpha) * avl_acc_z;	

}

void get_acc_x(void)
{	
		imu660rb_get_acc();
	 acc_x = imu660rb_acc_transition(-imu660rb_acc_x);

		avl_acc_x = acc_x;
    acc_x_filtered = alpha * acc_x_filtered + (1 - alpha) * avl_acc_x;	

}

void get_acc_y(void)
{	
		imu660rb_get_acc();
	 acc_y = imu660rb_acc_transition(-imu660rb_acc_y);

		avl_acc_y = acc_y;
    acc_y_filtered = alpha * acc_y_filtered + (1 - alpha) * avl_acc_y;	

}

void angle_clear(void)
{
    angle_z = 0;
    angle_x = 0;
}