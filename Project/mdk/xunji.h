#ifndef __XUNJI_H
#define __XUNJI_H

       
#include "bsp_system.h"

extern int16 correct_speed_L,correct_speed_R;



// 环岛状态变量声明


///extern int8 Angle_huan_flag1;        // 入环打角标志
///extern int8 Angle_huan_flag2;        // 环内循迹标志
///extern int8 Angle_huanout_flag2;     // 出环直行标志
///extern float straght;                 // 出环直行位移


//// 环岛参数（可根据实际调整）
//#define HUAN_TRIGGER_L    90   // 左内侧触发阈值
//#define HUAN_TRIGGER_R    90   // 右内侧触发阈值
//#define HUAN_RATIO         1.5f  // 内侧比值系数


//// 环岛角度参数
//#define HUAN_ANGLE_IN      10    // 入环打角角度
//#define HUAN_ANGLE_OUT     380   // 出环触发角度









// --- 环岛状态枚举 ---
typedef enum {
    ROUND_NONE = 0,   // 正常行驶
    ROUND_IN,         // 1. 入环打角
    ROUND_KEEP,       // 2. 环内循迹
    ROUND_PREOUT,     // 3. 准备出环
    ROUND_EXIT        // 4. 回正退出
} RoundState;

// --- 环岛参数阈值（根据实测微调） ---
#define HUAN_ANGLE_IN     40.0f   // 入环打角角度（建议10-20度，尽早进入循迹）
#define HUAN_ANGLE_OUT    280.0f  // 准备出环角度（约270-310度）
#define HUAN_TRIGGER_VAL  220     // 电感触发总强度阈值
#define EXIT_COOLDOWN_MS  1500    // 出环冷却时间（防止重复触发）

extern RoundState cur_state;
extern int8 huan_flag;              // 环岛触发标志
extern int8 direct_l;                // 左环方向标志
extern int8 direct_r;                // 右环方向标志







void xunji(void);
void huan_check(void);             // 环岛触发检测
void benhuan(void);									 // 环岛状态机
void huan_init(void);               // 环岛变量初始化
void display_huan_state(void);
#endif