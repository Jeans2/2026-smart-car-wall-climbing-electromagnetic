/*********************************************************************************************************************
* STC32G Opensourec Library 即（STC32G 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是STC 开源库的一部分
*
* STC32G 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MDK FOR C251
* 适用平台          STC32G
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者           备注
* 2024-08-01        大W            first version
********************************************************************************************************************/

#ifndef __BOARD_H
#define __BOARD_H
#include "zf_common_typedef.h"
#include "zf_driver_uart.h"


#define SYSTEM_CLOCK_22_1184M 	22118400
#define SYSTEM_CLOCK_24M      	24000000
#define SYSTEM_CLOCK_27M      	27000000
#define SYSTEM_CLOCK_30M      	30000000
#define SYSTEM_CLOCK_33_1776M 	33177600
#define SYSTEM_CLOCK_35M      	35000000


#define EXTERNAL_CRYSTA_ENABLE 	0			// 使用外部晶振，0为不使用，1为使用（建议使用内部晶振）

#define FOSC					0			// FOSC的值设置为0，则内核频率通过寄存器强制设置。
											// 不管STC-ISP软件下载时候选择多少，他都是设置的频率。
											
//#define FOSC      	SYSTEM_CLOCK_30M	// FOSC的值设置为30Mhz,
											// 使用STC-ISP软件下载的时候，
											// 此频率需要跟STC-ISP软件中的 <输入用户程序运行时的IRC频率>选项的频率一致。

extern int32 system_clock;

void clock_init (uint32 clock);                                               // 核心时钟初始化


#endif

