#include "imu660rb.h"
#include "math.h"
#include "zf_device_imu660rc.h"

static void gyro_get_z(void);
static void gyro_get_x(void);
static void get_acc_z(void);
static void get_acc_y(void);
static void get_acc_x(void);

static float null_drift_z = 0;
static float null_drift_x = 0;
static float gyro_z;
static float gyro_x;
float angle_z = 0;
static float angle_x = 0;
static float avl_gyro_z;
static float avl_gyro_x;
static float gyro_z_filtered = 0;
static float gyro_x_filtered = 0;
static const float alpha = 0.9f;

static float acc_z_filtered;
static float acc_z;
static float avl_acc_z;
static float acc_y_filtered;
static float acc_y;
static float avl_acc_y;
static float acc_x_filtered;
static float acc_x;
static float avl_acc_x;

float pitch;

void gyro_init_calibration(void)
{
	int i;
	float temp_z = 0, temp_x = 0;

	for (i = 0; i < 500; i++)
	{
		imu660rc_get_gyro();
		temp_z += imu660rc_gyro_transition(imu660rc_gyro_z);
		temp_x += imu660rc_gyro_transition(imu660rc_gyro_x);
		system_delay_ms(1);
	}
	null_drift_z = temp_z / 500.0f;
	null_drift_x = temp_x / 500.0f;
}

static void gyro_get_z(void)
{
	imu660rc_get_gyro();
	gyro_z = imu660rc_gyro_transition(imu660rc_gyro_z) - null_drift_z;

	if (gyro_z > -0.5f && gyro_z < 0.5f) gyro_z = 0;

	avl_gyro_z = gyro_z;
	gyro_z_filtered = alpha * gyro_z_filtered + (1.0f - alpha) * avl_gyro_z;
}

static void gyro_get_x(void)
{
	imu660rc_get_gyro();
	gyro_x = imu660rc_gyro_transition(imu660rc_gyro_x) - null_drift_x;

	if (gyro_x > -0.5f && gyro_x < 0.5f) gyro_x = 0;

	avl_gyro_x = gyro_x;
	gyro_x_filtered = alpha * gyro_x_filtered + (1.0f - alpha) * avl_gyro_x;
}

void angle_get(void)
{
	gyro_get_z();
	gyro_get_x();
	get_acc_z();
	get_acc_y();
	get_acc_x();

	angle_x += 0.002f * gyro_x_filtered;
	angle_z += 0.002f * gyro_z_filtered;

	pitch = atan2(acc_y_filtered, acc_z_filtered) * 57.3f;
}

void angle_clear(void)
{
	angle_z = 0;
	angle_x = 0;
}

static void get_acc_z(void)
{
	imu660rc_get_acc();
	acc_z = imu660rc_acc_transition(-imu660rc_acc_z);
	avl_acc_z = acc_z;
	acc_z_filtered = alpha * acc_z_filtered + (1.0f - alpha) * avl_acc_z;
}

static void get_acc_x(void)
{
	imu660rc_get_acc();
	acc_x = imu660rc_acc_transition(-imu660rc_acc_x);
	avl_acc_x = acc_x;
	acc_x_filtered = alpha * acc_x_filtered + (1.0f - alpha) * avl_acc_x;
}

static void get_acc_y(void)
{
	imu660rc_get_acc();
	acc_y = imu660rc_acc_transition(-imu660rc_acc_y);
	avl_acc_y = acc_y;
	acc_y_filtered = alpha * acc_y_filtered + (1.0f - alpha) * avl_acc_y;
}
