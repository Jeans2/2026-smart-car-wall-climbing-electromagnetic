#ifndef __KEY_H
#define __KEY_H


#define KP_STEP 0.1f   // 每次按键增减的步长
#include "bsp_system.h"

#define KEY1_PIN    IO_P70  // 控制 Kp 增加   切换光标
#define KEY2_PIN    IO_P71  // 控制 Kp 减小		KEY2: 增加当前参数
#define KEY3_PIN    IO_P72  // 控制 Kd 增加		KEY3: 减小当前参数
#define KEY4_PIN    IO_P73  // 控制 Kd 减小		KEY4: 备用按键（预留给写入 Flash）

#define KP_MAX  5.0f   // Kp的最大允许值
#define KP_MIN  0.0f   // Kp的最小允许值 (通常不能为负)


void key_init(void);
#endif