#include "aoi_function.h"


int32 abs_int32(int32 num)
{
	if(num<0)
	{
		num = -num;
	}

	return num;
}




// 交换两个整数（如果需要）
void compare_swap(uint16 *a, uint16 *b) {
	uint16 t;
    if (*a > *b) {
         t = *a;
        *a = *b;
        *b = t;
    }
}

// 高效排序7个数的函数（手动展开比较网络）
uint16 sort_seven(uint16 arr[7]) {
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
//滑动平均滤波 初始化
void initSlidingAverage(SlidingAverageFilter* filter, int N) {
	 uint8 Win_i=0;
    if (N > MAX_WINDOW_SIZE) {
        N = MAX_WINDOW_SIZE; // 限制窗口大小不能超过最大值
    }

    for (Win_i = 0; Win_i < MAX_WINDOW_SIZE; Win_i++) {
        filter->buffer[Win_i] = 0.0f;
    }
    filter->sum = 0.0f;
    filter->index = 0;
    filter->count = 0;
    filter->window_size = N;
	}		
//滑动平均滤波
void slidingAverage(float now_speed, float* avg_speed, SlidingAverageFilter* filter) {
    // 从和中减去即将被替换的旧值
    filter->sum -= filter->buffer[filter->index];

    // 将新的值插入缓冲区
    filter->buffer[filter->index] = now_speed;

    // 更新和
    filter->sum += now_speed;

    // 更新索引，使其循环回到起始位置
    filter->index = (filter->index + 1) % filter->window_size;

    // 如果窗口没有满，增加计数
    if (filter->count < filter->window_size) {
        filter->count++;
    }

    // 计算并返回当前窗口的平均值
    *avg_speed = filter->sum / filter->count;
}
	
		
		
