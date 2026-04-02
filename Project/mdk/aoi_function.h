#ifndef __AOI_FUNCTION_H
#define __AOI_FUNCTION_H

#include "bsp_system.h"


int32 abs_int32(int32 num);


#define MAX_WINDOW_SIZE 3

void compare_swap(uint16 *a, uint16 *b);
uint16 sort_seven(uint16 arr[7]);
// 滑动平均滤波器结构体
typedef struct {
    float buffer[MAX_WINDOW_SIZE]; // 静态缓冲区
    float sum;                     // 当前窗口数值的和
    int index;                     // 当前存储数值的位置
    int count;                     // 当前已填充的数值个数
    int window_size;               // 滑动窗口大小
} SlidingAverageFilter;
void initSlidingAverage(SlidingAverageFilter* filter, int N);
void slidingAverage(float now_speed, float* avg_speed, SlidingAverageFilter* filter);


#endif
