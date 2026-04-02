#include "pid.h" 

//P I D 							
struct PID pid_motor_straight={0,0,0,  0.38,0,0.33};//0.38 0.25			//循迹 // 直道转向环 角速度环参数
struct PID pid_motor_turn=		{180,0,60   ,16,0,20};	    //12 3 // 弯道参数
struct PID pid_motor_ringR=		{750,0,45   ,16,0,20};      //苯环参数

struct PID pid_loop_speed_start=		{10,3};										//速度环
struct PID pid_loop_speed_run=	  	{20,5};										//速度环

struct PID pid_loop_angle_ring=		{50,0,0   ,16,0,20};				//角度环

struct PID pid_loop_gyro=		  {0,0,0   ,16,0,20};					//角速度环

float limit_gyro = 800;



//2-循迹
struct PID pid_motor_run;
















// 全局变量定义

//速度环
int16 err_speed=0;
int16 speed_target = 0;
float out_L=0,out_R=0;
struct PID pid_loop_speed = {40,0.125,0};			//40,0.125,0   2ms中断下
//struct PID pid_loop_speed = {60,0.37,0};          //右轮
//角速度环
float expect_gyro = 0;  // 角速度期望值
float correct_L = 0;    // 电机差速修正值
//float avl_gyro_z = 0;    实际角速度（需要从传感器获取）已定义
static float direction_err2[2] = {0};
struct PID pid_motor_run = {1.54,1.5,0,  0,0,0};// 1.5 4.0               0.55 0.04             95速度---3.6




// 方向环（位置环）相关变量
static float direction_err1[4] = {0};
static float D_err = 0;
static int flag = 0;













//========================================================================
// 速度环（内环）
//========================================================================
void speed_loop_LR(int16 speed_L_t,int16 speed_R_t)
{
	//0-编码器输入更新
	static int16 err_speed_L_last=0,err_speed_L=0;
	static float speed_output_L=0;
	static int16 err_speed_R_last=0,err_speed_R=0;
	static float speed_output_R=0;
	
	//1-误差计算
	err_speed_L_last = err_speed_L;
	err_speed_L = speed_L_t - speed_L;
	
	
	
	//2-控制
	speed_output_L = pid_loop_speed.Kp * (err_speed_L - err_speed_L_last)
									  + pid_loop_speed.Ki * err_speed_L;

	//增量
	if(speed_output_L>2000 ){speed_output_L=2000; }
	if(speed_output_L<-2000){speed_output_L=-2000;}
	
	//输出
	out_L += speed_output_L;
	
	
	
	//0-编码器输入更新
	
	//1-误差计算
	err_speed_R_last = err_speed_R;
	err_speed_R = speed_R_t - speed_R;
	
	//2-控制
	speed_output_R =  pid_loop_speed.Kp * (err_speed_R - err_speed_R_last)
									  + pid_loop_speed.Ki *  err_speed_R;

	//增量
	if(speed_output_R>2000 ){speed_output_R=2000; }
	if(speed_output_R<-2000){speed_output_R=-2000;}
	
	//输出
	out_R += speed_output_R;
	
  if(out_L>3000 ){out_L=3000; }
	if(out_L<-3000){out_L=-3000;}
	if(out_R>3000 ){out_R=3000; }
	if(out_R<-3000){out_R=-3000;}
}




////========================================================================
//// 角速度环 （中环）- 输入角速度期望，输出电机差速修正值
////========================================================================
//float gyro_loop(float expect_gyro, float avl_gyro_z)
//{
//    float err_gyro;
//    
//    // 计算角速度误差
//    err_gyro = expect_gyro - avl_gyro_z;
//    
//    // 更新误差数组
//    direction_err2[1] = direction_err2[0];
//    direction_err2[0] = err_gyro;

//    // PD控制器计算修正量
//    correct_L = pid_motor_run.Kp_gyro * direction_err2[0]
//              + pid_motor_run.Kd_gyro * (direction_err2[0] - direction_err2[1]);
//    
//    // 限幅
//    if(correct_L > 10000) correct_L = 10000;
//    if(correct_L < -10000) correct_L = -10000;
//    
//    return correct_L;  // 返回电机差速修正值
//}


//========================================================================
// 方向环（位置环）（外环）- 输入位置误差，输出角速度期望
//========================================================================
float direction_loop(float err_position)
{
    static float err_feedforward = 0, err_gyro, dec = 0;
    static float expect_gyro_last = 0, correct_L_last = 0;
    
    // 更新位置误差数组（1阶或2阶滤波）
    direction_err1[2] = direction_err1[1];
    direction_err1[1] = direction_err1[0];
    direction_err1[0] = err_position;

    // 计算误差变化量
    D_err = direction_err1[0] - direction_err1[1];
    
    // 抗干扰处理：如果误差突变过大，认为是干扰，保持上次值
//    if(flag == 0 && (D_err > 5.3 || D_err < -5.3))
//    {
//        direction_err1[0] = direction_err1[1];
//        flag = 1;
//    }
//    else
//    {
//        flag = 0;
//    }
    
    // PD控制器计算角速度期望
    correct_L = pid_motor_run.Kp * direction_err1[0] 
                + pid_motor_run.Kd * (direction_err1[0] - direction_err1[1]); // 可以根据需要添加direction_err1[3]
    
    // 限幅
    if(correct_L > 10000) correct_L = 10000;
    if(correct_L < -10000) correct_L = -10000;
    
    return correct_L;  // 返回角速度期望值
}








////========================================================================
//// 角速度环 （中环）- 输入角速度期望，输出电机差速修正值
////========================================================================
//float gyro_loop(float expect_gyro, float avl_gyro_z)
//{
//    float err_gyro;
//    
//    // 计算角速度误差
//    err_gyro = expect_gyro - avl_gyro_z;
//    
//    // 更新误差数组
//    direction_err2[1] = direction_err2[0];
//    direction_err2[0] = err_gyro;

//    // PD控制器计算修正量
//    correct_L = pid_motor_run.Kp_gyro * direction_err2[0]
//              + pid_motor_run.Kd_gyro * (direction_err2[0] - direction_err2[1]);
//    
//    // 限幅
//    if(correct_L > 10000) correct_L = 10000;
//    if(correct_L < -10000) correct_L = -10000;
//    
//    return correct_L;  // 返回电机差速修正值
//}


////========================================================================
//// 方向环（位置环）（外环）- 输入位置误差，输出角速度期望
////========================================================================
//float direction_loop(float err_position)
//{
//    static float err_feedforward = 0, err_gyro, dec = 0;
//    static float expect_gyro_last = 0, correct_L_last = 0;
//    
//    // 更新位置误差数组（1阶或2阶滤波）
//    direction_err1[2] = direction_err1[1];
//    direction_err1[1] = direction_err1[0];
//    direction_err1[0] = err_position;

//    // 计算误差变化量
//    D_err = direction_err1[0] - direction_err1[1];
//    
//    // 抗干扰处理：如果误差突变过大，认为是干扰，保持上次值
////    if(flag == 0 && (D_err > 5.3 || D_err < -5.3))
////    {
////        direction_err1[0] = direction_err1[1];
////        flag = 1;
////    }
////    else
////    {
////        flag = 0;
////    }
//    
//    // PD控制器计算角速度期望
//    correct_L = pid_motor_run.Kp * direction_err1[0] 
//                + pid_motor_run.Kd * (direction_err1[0] - direction_err1[1]); // 可以根据需要添加direction_err1[3]
//    
//    // 限幅
//    if(expect_gyro > limit_gyro) expect_gyro = limit_gyro;
//    if(expect_gyro < -limit_gyro) expect_gyro = -limit_gyro;
//    
//    return expect_gyro;  // 返回角速度期望值
//}















































































