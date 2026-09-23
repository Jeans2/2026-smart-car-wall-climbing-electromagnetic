#include "imu660rb.h"
#include "zf_device_imu660rc.h"

static void gyro_get_z(void);

static float null_drift_z = 0;
static float gyro_z;
float angle_z = 0;
static float avl_gyro_z;
static float gyro_z_filtered = 0;
float gyro_z_turn = 0;
static const float alpha = 0.9f;

float pitch;

void gyro_init_calibration(void)
{
	int i;
	float temp_z = 0;

	system_delay_ms(100);  // 等待四元数回调开始填充原始数据
	for (i = 0; i < 500; i++)
	{
		temp_z += imu660rc_gyro_transition(imu660rc_gyro_z);
		system_delay_ms(1);
	}
	null_drift_z = temp_z / 500.0f;
}

static void gyro_get_z(void)
{
	// 四元数模式下原始数据由 INT0 回调更新，直接读全局变量
	gyro_z = imu660rc_gyro_transition(imu660rc_gyro_z) - null_drift_z;

	if (gyro_z > -0.5f && gyro_z < 0.5f) gyro_z = 0;

	avl_gyro_z = gyro_z;
	gyro_z_filtered = alpha * gyro_z_filtered + (1.0f - alpha) * avl_gyro_z;
	gyro_z_turn = gyro_z_filtered;
}



void angle_get(void)
{
	gyro_get_z();
	angle_z += 0.002f * gyro_z_filtered;         // 保留：环岛需要 angle_z 积分
	pitch  = imu660rc_pitch;                      // 改用 RC 硬件四元数俯仰角
}

void angle_clear(void)
{
	angle_z = 0;
}

