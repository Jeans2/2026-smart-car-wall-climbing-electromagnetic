#include "pid.h" 

//速度环
int16 err_speed=0;
int16 speed_target = 0;
float out_L=0,out_R=0;
struct PID pid_loop_speed = {10,0.005};			







//直行速度环
void speed_loop(void)
{
		static int16 last_err_speed=0;
		static float speed_output=0;//总输出
	//1-误差计算
   last_err_speed = err_speed;
	 err_speed = speed_target - speed_avl;
	
//2—增量式PI控制
		speed_output =  pid_loop_speed.Kp * (err_speed - last_err_speed)
									+ pid_loop_speed.Ki *  err_speed;
	
	//增量
	if(speed_output>2000 ){speed_output=2000; }
	if(speed_output<-2000){speed_output=-2000;}
	
	//输出
	out_L += speed_output;
	out_R = out_L;
	
	if(out_L>9900 ){out_L=9900; }
	if(out_L<-9900){out_L=-9900;}
	if(out_R>9900 ){out_R=9900; }
	if(out_R<-9900){out_R=-9900;}	
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






