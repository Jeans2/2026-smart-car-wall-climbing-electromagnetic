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
