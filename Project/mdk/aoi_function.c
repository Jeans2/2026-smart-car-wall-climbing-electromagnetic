#include "aoi_function.h"

int32 abs_int32(int32 num)
{
	if (num < 0) num = -num;
	return num;
}

static void compare_swap(uint16 *a, uint16 *b)
{
	if (*a > *b)
	{
		uint16 t = *a;
		*a = *b;
		*b = t;
	}
}

uint16 sort_seven(uint16 arr[7])
{
	compare_swap(&arr[0], &arr[1]);
	compare_swap(&arr[2], &arr[3]);
	compare_swap(&arr[4], &arr[5]);
	compare_swap(&arr[0], &arr[2]);
	compare_swap(&arr[1], &arr[3]);
	compare_swap(&arr[2], &arr[4]);
	compare_swap(&arr[3], &arr[5]);
	compare_swap(&arr[5], &arr[6]);
	compare_swap(&arr[3], &arr[5]);
	compare_swap(&arr[1], &arr[3]);
	compare_swap(&arr[0], &arr[1]);
	compare_swap(&arr[2], &arr[3]);
	compare_swap(&arr[4], &arr[5]);
	compare_swap(&arr[1], &arr[2]);
	compare_swap(&arr[3], &arr[4]);
	compare_swap(&arr[2], &arr[3]);
	compare_swap(&arr[6], &arr[5]);
	compare_swap(&arr[5], &arr[4]);
	compare_swap(&arr[4], &arr[3]);
	compare_swap(&arr[3], &arr[2]);
	compare_swap(&arr[2], &arr[1]);
	compare_swap(&arr[1], &arr[0]);
	compare_swap(&arr[0], &arr[1]);
	compare_swap(&arr[1], &arr[2]);
	compare_swap(&arr[2], &arr[3]);
	compare_swap(&arr[3], &arr[4]);
	compare_swap(&arr[4], &arr[5]);
	compare_swap(&arr[5], &arr[6]);

	return arr[3];
}

void initSlidingAverage(SlidingAverageFilter* filter, int N)
{
	uint8 i;

	if (N > MAX_WINDOW_SIZE)
		N = MAX_WINDOW_SIZE;

	for (i = 0; i < MAX_WINDOW_SIZE; i++)
		filter->buffer[i] = 0.0f;
	filter->sum = 0.0f;
	filter->index = 0;
	filter->count = 0;
	filter->window_size = N;
}

void slidingAverage(float now_speed, float* avg_speed, SlidingAverageFilter* filter)
{
	filter->sum -= filter->buffer[filter->index];
	filter->buffer[filter->index] = now_speed;
	filter->sum += now_speed;
	filter->index = (filter->index + 1) % filter->window_size;

	if (filter->count < filter->window_size)
		filter->count++;

	*avg_speed = filter->sum / filter->count;
}
