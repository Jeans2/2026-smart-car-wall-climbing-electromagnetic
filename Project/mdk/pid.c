#include "pid.h" 

//速度环
int16 err_speed=0;
int16 speed_target = 0;
float out_L=0,out_R=0;
struct PID pid_loop_speed = {40,0.125,0};			
struct PID pid_loop_angle = {0,0,0};
struct PID pid_motor_run = {0,0,0};





//直行速度环
void speed_loop(void)
{
		static int16 last_err_speed=0;
	  static int16 last_last_err_speed=0;
		static float speed_output=0;//总输出
	//1-误差计算
   last_last_err_speed = last_err_speed;
	 last_err_speed = err_speed; 
	 err_speed = speed_target - speed_avl;
	
//2—增量式PI控制
		speed_output =  pid_loop_speed.Kp * (err_speed - last_err_speed)
									+ pid_loop_speed.Ki *  err_speed 
									+ pid_loop_speed.Kd * (err_speed - 2*last_err_speed + last_last_err_speed);
	
	//增量
	if(speed_output>2000 ){speed_output=2000; }
	if(speed_output<-2000){speed_output=-2000;}
	
	//输出
	out_L+= speed_output;
	out_R = out_L;
	
	if(out_L>5000 ){out_L=5000; }
	if(out_L<-5000){out_L=-5000;}
	if(out_R>5000 ){out_R=5000; }
	if(out_R<-5000){out_R=-5000;}	
}


//左右轮独立速度环测试
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
	if(speed_output_L>3000 ){speed_output_L=3000; }
	if(speed_output_L<-3000){speed_output_L=-3000;}
	
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
	if(speed_output_R>3000 ){speed_output_R=3000; }
	if(speed_output_R<-3000){speed_output_R=-3000;}
	
	//输出
	out_R += speed_output_R;
	
  if(out_L>9900 ){out_L=9900; }
	if(out_L<-9900){out_L=-9900;}
	if(out_R>9900 ){out_R=9900; }
	if(out_R<-9900){out_R=-9900;}
}



float limit_gyro =6  ;                               ////后期修改
float expect_gyro;
float correct_L;
void direction_return(float err_position)
{
	static float err_feedforward=0, err_gyro,dec=0;
	static float direction_err1[4]={0}, direction_err2[2]={0};
	static float expect_gyro_last=0,correct_L_last=0;
	static float D_err=0;
	static int flag=0;
	//1-电磁位置->方向环->角速度期望
//	direction_err1[3]=direction_err1[2];
	direction_err1[2]=direction_err1[1];
	direction_err1[1]=direction_err1[0];
	direction_err1[0]=err_position;

	
	D_err = direction_err1[0] - direction_err1[1];
	
	if(flag==0 &&(D_err>5.3||D_err<-5.3))
	{
		direction_err1[0] = direction_err1[1];
		flag=1;
	}
	else
	{
		flag=0;
	}
	
	//P D
	expect_gyro =  pid_motor_run.Kp *  direction_err1[0]		
							 + pid_motor_run.Kd * (direction_err1[0] - direction_err1[1]);    //之后再测direction_err1[3]) 

	
	if(expect_gyro>limit_gyro){expect_gyro=limit_gyro;}
	if(expect_gyro<-limit_gyro){expect_gyro=-limit_gyro;}
	
	//2-角速度->角速度环->电机差速(值)
	err_gyro = expect_gyro - avl_gyro_z;
	direction_err2[1]=direction_err2[0];
	direction_err2[0]=err_gyro;

	correct_L	=  pid_motor_run.Kp_gyro *  direction_err2[0]
						 + pid_motor_run.Kd_gyro * (direction_err2[0] - direction_err2[1]);
	
	 
	
	if(correct_L>10000){correct_L=10000;}
	if(correct_L<-10000){correct_L=-10000;}

	correct_L_last = correct_L;

}

//角度环
float expect_gyro_angles;
float out_angle;
int16 speed_turn = 6 ;
void loop_angle(float next_angle,float target_angle)
{	
	static float err_gyro;
	static float gyro_err[2]={0},angle_err[3]={0};
	
	//角度环
	angle_err[2] = angle_err[1];
	angle_err[1] = angle_err[0];
	angle_err[0] = target_angle - next_angle;			//误差更新
		
	expect_gyro_angles =  pid_loop_angle.Kp *  angle_err[0] 
							 + pid_loop_angle.Kd * (angle_err[1] - angle_err[2]);		//PID
	
	//输出限幅
	if(expect_gyro_angles>speed_turn) {expect_gyro_angles=speed_turn;}
	if(expect_gyro_angles<-speed_turn){expect_gyro_angles=-speed_turn;}	
	
	//角速度环
	err_gyro = expect_gyro_angles - avl_gyro_z;
	gyro_err[1]=gyro_err[0];
	gyro_err[0]=err_gyro;	

	out_angle	=    pid_loop_angle.Kp_gyro *  gyro_err[0]
							 + pid_loop_angle.Kd_gyro * (gyro_err[0] - gyro_err[1]);
//						 	 + err_gyro*err_gyro*err_gyro*0.0001; 	

	if(correct_L>12500){correct_L=12500;}
	if(correct_L<-12500){correct_L=-12500;}

}





