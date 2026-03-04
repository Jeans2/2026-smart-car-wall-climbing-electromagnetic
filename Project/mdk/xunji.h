#ifndef __XUNJI_H
#define __XUNJI_H

//1.检测传感器
//2.传感器数据分析
//3.反馈环
//4.输出数据驱动小车           
#include "bsp_system.h"

extern int16 correct_speed_L,correct_speed_R;


void xunji(void);
// 环岛状态变量声明
extern int8 huan_flag;              // 环岛触发标志
extern int8 direct_l;                // 左环方向标志
extern int8 direct_r;                // 右环方向标志
extern float Angle_huan;             // 环岛角度积分
extern int8 Angle_huan_flag1;        // 入环打角标志
extern int8 Angle_huan_flag2;        // 环内循迹标志
extern int8 Angle_huanout_flag2;     // 出环直行标志
extern float straght;                 // 出环直行位移


// 环岛参数（可根据实际调整）
#define HUAN_TRIGGER_LM    200   // 左内侧触发阈值
#define HUAN_TRIGGER_RM    200   // 右内侧触发阈值
#define HUAN_RATIO         1.5f  // 内侧比值系数


// 环岛角度参数
#define HUAN_ANGLE_IN      40    // 入环打角角度
#define HUAN_ANGLE_OUT     300   // 出环触发角度



void xunji(void);
void huan_check(void);             // 环岛触发检测
void benhuan(void);									 // 环岛状态机
void huan_init(void);               // 环岛变量初始化
#endif