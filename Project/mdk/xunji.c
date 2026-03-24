#include "xunji.h"


float abs_angle = 0;
int16 speed_straight = 65;
int16 speed_ringR = 60;
int8  element = 0;
int8 turn_flag=0;
//int16 Ring_choice=1;

float distance_ringR = 0;       // 距离积分
float distance_ringR_1 = 8000;     // 打角点前直道路距离
float distance_ringL_1 = 10000;     // 打角点前直道路距离





//环岛状态标志
int16 ring_flag_ing=0;//是否处于环岛任务的状态中，初始为0 
int16 ringR_flag_task=1;// 任务决策状态 (0:未遇到环岛, 1~5:环岛各阶段)
int16 ringR_flag_execute=1;

	
void xunji(void) 
{
//	if(L > 75&&(R+RM+L+LM)>180&&ring_flag_ing==0)	//左环岛
//	{
////		if(Ring_choice == 1){element = 2;}
////		else(element = 4);
//		element = 2;
//		turn_flag = 4;
//		
//	} 
	
	if(R > 80 &&(R+RM+L+LM)>180&&ring_flag_ing==0)	//右环岛
	{
//		if(Ring_choice == 1){element = 1;}
//		else(element = 3);
		element = 1;
		turn_flag = 5;
	}
	if(element==1&& ringR_flag_task == 1)
	{
			if((R+RM+L+LM)<110)
			{
						element=0;
						angle_clear();
				distance_ringR=0;
				ringR_flag_task=1;
				ringR_flag_execute=1;				
				
			
			}
	
	
	
	}
//	if(L+LM+RM+R<20)									//出界保护
//	{element = 9;			turn_flag = 9;}
	switch (element)
	{
		case 0:												//**正常循迹**//直道路
			turn_flag=0;
//			pid_motor_run=pid_motor_straight;  //调用直道pid数值
			speed_target = speed_straight;	
			
//			direction_return(deviation);//得到差速correct_L
				speed_loop_LR(speed_target + correct_L,speed_target - correct_L);		     		   	    
			  set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);

		
			if(ring_flag_ing>0)
			{
			 ring_flag_ing--;
			}
			  break;
			
//				/**环岛任务**/
		case 1:							//右环岛				
			ringR_task();
			ringR_execute();
			break;
			
		
//		case 2:							//左环岛
//			ringL_task();
//			ringL_execute();
//			break;
//		case 3:
//			ringR_task_trace();
//			ringR_execute_trace();
//			break;
//		case 4:
//			ringL_task_trace();
//			ringL_execute_trace();
//			break;
		case 9:
			set_pwm_motor_L(0);
			set_pwm_motor_R(0);
			break;
		default:break;
		
	}
}





















//	/***************元素执行**************/
//环岛状态判断
void ringR_task(void)
{
    switch(ringR_flag_task)
    {
        case 1:                                     // 1-准备进岛
            ring_flag_ing = 200;                    // 进入环岛状态
            ringR_flag_execute = 1;
            distance_ringR += speed_avl;
            if(distance_ringR > distance_ringR_1)   // 到达打角点
            {   
                angle_clear();                      // 到达打角点，角度清零
                distance_ringR = 0;
                ringR_flag_task = 2;
                ringR_flag_execute = 2; 
            }
            break;
            
        case 2:                                     // 2-进岛打角角度积分
            if(angle > 30)                          // 打角完成
            {
                ringR_flag_task = 3;
                ringR_flag_execute = 3;
            }
            break;
                    
        case 3:                                     // 3-岛内电磁循迹角度积分       
           
            if(angle > 300)                         // 到达出环角度，直接触发直行出岛
            {                           
                ringR_flag_task = 4;
                ringR_flag_execute = 4;
            }
            break;
                        
        case 4:                                     // 4-直线出岛 (原case 5)
            distance_ringR += speed_avl;
            if(distance_ringR > 8000)               // 跑出足够距离，完全出岛
            {   
                angle_clear();
                distance_ringR = 0;
                ringR_flag_task = 1;
                ringR_flag_execute = 1;
                element = 0;                        // 元素清零
            }
            break;
                                
        default:
            break;  
    }
}



void ringL_task(void)
{
	switch(ringR_flag_task)
	{
		case 1:													//1准备进岛
			ring_flag_ing =200;//进入环岛状态
			ringR_flag_execute=1;
			distance_ringR += speed_avl;
			if(distance_ringR>distance_ringL_1)							//2到达打角点
			{	
				angle_clear();				// 到达打角点，角度清零
				
				distance_ringR=0;
				ringR_flag_task=2;
				ringR_flag_execute=2;	
					
			}
			break;
			
		case 2:								
											//1进岛角度积分
		
			if(angle<-80)						//2打角完成
			{
				ringR_flag_task=3;
				ringR_flag_execute=3;
			}
			break;
					
		case 3:
											//1岛内角度积分
		
			if(angle<-260)					//2即将到达打角点
			{							//335
				ringR_flag_task=4;
				ringR_flag_execute=4;
			}
			break;
						
		case 4:
											//1岛内角度积分
			if(angle<-340)			
			{
				
				ringR_flag_task=5;
				ringR_flag_execute=5;
			}
			break;
		
		case 5:
			
			distance_ringR += speed_avl;
			
			if(distance_ringR>5000)							//2到达打角点
			{	
				angle_clear();
				distance_ringR=0;
				ringR_flag_task=1;
				ringR_flag_execute=1;
				element=0;
				distance_ringR=0;
			}
								
		default:
			break;	
	}
}


//环岛执行
void ringR_execute(void)
{
    float target_L = 0;
    float target_R = 0;
    
    switch(ringR_flag_execute)
    {
        case 1: // 1-直线进岛 
            speed_target = speed_ringR;
            target_L = speed_target;
            target_R = speed_target;        
            break;
        
        case 2: // 2-打角进岛 
            // 左轮加速推，右轮减速拉。
            speed_target = speed_ringR;
            target_L = speed_target + 50; 
            target_R = speed_target - 30;
            break;
        
        case 3: // 3-岛内电磁巡线
            // 车进岛了，恢复差比和差公式，让电感带着车跑圆圈
            speed_target = speed_ringR;
            target_L = speed_target + correct_L; 
            target_R = speed_target - correct_L;
            break;
        
        case 4: // 4-直线出岛 (原case 5，锁死两轮相同速度直线跑开)
            speed_target = speed_straight;  // 注意：这里恢复为你原来的 speed_straight
            target_L = speed_target;
            target_R = speed_target;
            break;
        
        default:
            break;
    }

    if(ringR_flag_execute != 0)
    {
        // 只有在环岛状态时，才执行环岛的速度分配
        speed_loop_LR(target_L, target_R);
        set_pwm_motor_R(out_R);
        set_pwm_motor_L(out_L);
    }
}




void ringL_execute(void)
{
    float target_L = 0;
    float target_R = 0;
    switch(ringR_flag_execute)
    {
        case 1: // 1-直线进岛 
						speed_target=speed_ringR;
						target_L = speed_target;
            target_R = speed_target;  		
						
            break;
        
        case 2: // 2-打角进岛 
            // 左轮减速，右轮加速。
						speed_target=speed_ringR;
						target_L = speed_target -50; 
            target_R = speed_target +40;
           
										
            break;
        
        case 3: // 3-岛内电磁巡线
            // 车进岛了，此时恢复差比和差公式，让电感带着车跑圆圈
						speed_target=speed_ringR;
						target_L = speed_target + correct_L; 
            target_R = speed_target - correct_L;
								          						
            break;
        
        case 4: // 4-固定差速打角出岛
            // 再次屏蔽电磁，给固定差速强行扭出环岛
						speed_target=speed_ringR;
            target_L = speed_target - 50;
            target_R = speed_target + 20;
            break;
        
        case 5: // 5-直线出岛 (与状态1一致，锁死直线跑开)
						
            speed_target=speed_straight;
						target_L = speed_target;
            target_R = speed_target;
													
            break;
        
        default:
            break;
    }

    
    if(ringR_flag_execute != 0)
    {
        // 只有在环岛状态时，才执行环岛的速度分配
        speed_loop_LR(target_L, target_R);
        set_pwm_motor_R(out_R);
				set_pwm_motor_L(out_L);
    }
}



















































































































//环岛任务执行
//void ringR_execute(void)
//{
//	static float expect_gyro;
//	switch(ringR_flag_execute){
//		case 1:															//1-直线x进岛			
//			speed_target=speed_ringR;					
//			gyro_loop(0);
//			if(out_gyro>0)
//			{
//				loop_speed_LR(speed_target + small_t*out_gyro,speed_target - large_t*out_gyro);
//			}
//			else
//			{
//				loop_speed_LR(speed_target + large_t*out_gyro,speed_target - small_t*out_gyro);
//			}
//			motor_L(out_L);
//	    motor_R(out_R);
//			break;
//		
//    case 2:															//2-打角进岛
//			speed_target = speed_ringR;			
//			gyro_loop(gyro_ring_in);		
//		
//			if(out_gyro>0)
//			{
//				loop_speed_LR(speed_target + small_t*out_gyro,speed_target - large_t*out_gyro);
//			}
//			else
//			{
//				loop_speed_LR(speed_target + large_t*out_gyro,speed_target - small_t*out_gyro);
//			}

//			motor_L(out_L);
//	    motor_R(out_R);
//			break;
//		
//		case 3:															//3-角速度环
//			speed_target = speed_ringR;			
//			gyro_loop(gyro_ring_middle);		
//		
//			if(out_gyro>0)
//			{
//				loop_speed_LR(speed_target + small_t*out_gyro,speed_target - large_t*out_gyro);
//			}
//			else
//			{
//				loop_speed_LR(speed_target + large_t*out_gyro,speed_target - small_t*out_gyro);
//			}

//			motor_L(out_L);
//	    motor_R(out_R);
//			break;
//		
//		case 4:															//固定角速度出岛
//			speed_target = speed_ringR;			
//			gyro_loop(gyro_ring_out);		
//		
//			if(out_gyro>0)
//			{
//				loop_speed_LR(speed_target + small_t*out_gyro,speed_target - large_t*out_gyro);
//			}
//			else
//			{
//				loop_speed_LR(speed_target + large_t*out_gyro,speed_target - small_t*out_gyro);
//			}
//			motor_L(out_L);
//	    motor_R(out_R);
//			break;
//		
//		case 5:															//直线x出岛
//			speed_target=speed_straight;					
//			gyro_loop(0);
//			if(out_gyro>0)
//			{
//				loop_speed_LR(speed_target + small_t*out_gyro,speed_target - large_t*out_gyro);
//			}
//			else
//			{
//				loop_speed_LR(speed_target + large_t*out_gyro,speed_target - small_t*out_gyro);
//			}			motor_L(out_L);
//	    motor_R(out_R);
//		
//		default:break;
//	}
//}



