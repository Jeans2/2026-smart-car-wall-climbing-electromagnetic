#ifndef __KEY_H
#define __KEY_H


#define KP_STEP 0.1f   // ÿ�ΰ��������Ĳ���
#include "bsp_system.h"

#define KEY1_PIN    IO_P70  //    �л����
#define KEY2_PIN    IO_P71  // 		KEY2: ���ӵ�ǰ����
#define KEY3_PIN    IO_P72  // 		KEY3: ��С��ǰ����
#define KEY4_PIN    IO_P73  // KEY4: 返回上一级
#define KEY5_PIN    IO_P75  // KEY5: 确认/进入/发车/停车

#define KP_MAX  5.0f   // 
#define KP_MIN  0.0f   // 


void key_init(void);
#endif