#ifndef __AOI_FUNCTION_H
#define __AOI_FUNCTION_H

#include "bsp_system.h"

#define MAX_WINDOW_SIZE 3

typedef struct {
	float buffer[MAX_WINDOW_SIZE];
	float sum;
	int index;
	int count;
	int window_size;
} SlidingAverageFilter;

int32 abs_int32(int32 num);
uint16 sort_seven(uint16 arr[7]);
void initSlidingAverage(SlidingAverageFilter* filter, int N);
void slidingAverage(float now_speed, float* avg_speed, SlidingAverageFilter* filter);

#endif
