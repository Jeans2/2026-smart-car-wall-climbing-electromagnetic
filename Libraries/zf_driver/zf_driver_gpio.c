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
* 2024-xx-xx        优化           精简重复逻辑，适配SFR特性
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_gpio.h"


// 内部使用，用户无需关心。
static void gpio_set_mode(gpio_pin_enum pin, gpio_mode_enum mode)
{
	#define PXPU(pin)  (*(unsigned char volatile far *)(0x7efe10 + (pin >> 4 - 1)))
//	#define PXPD(pin)  (*(unsigned char volatile far *)(0x7efe40 + (pin >> 4 - 1)))
	
    if(GPI_FLOATING_IN == mode || GPIO_NO_PULL == mode)
    {
        PXPU(pin) &= ~(1 << (pin & 0x0F));
//        PXPD(pin) &= ~(1 << (pin & 0x0F));
    }
//    else if(GPI_PULL_DOWN == mode)
//    {
//        PXPD(pin) |= (1 << (pin & 0x0F));
//    }
    else if(GPI_PULL_UP == mode || GPO_PUSH_PULL == mode)
    {
        PXPU(pin) |= (1 << (pin & 0x0F));
    }
	else if(GPO_OPEN_DTAIN == mode)
	{
        // 开漏输出：操作对应端口的模式寄存器（PnM1/PnM0）
		switch(pin & 0xF0)
		{
			case IO_P00:
                P0M1 |= (1 << (pin & 0xF));
                P0M0 |= (1 << (pin & 0xF));
				break;
			case IO_P10:
                P1M1 |= (1 << (pin & 0xF));
                P1M0 |= (1 << (pin & 0xF));
				break;
			case IO_P20:
                P2M1 |= (1 << (pin & 0xF));
                P2M0 |= (1 << (pin & 0xF));
				break;
			case IO_P30:
                P3M1 |= (1 << (pin & 0xF));
                P3M0 |= (1 << (pin & 0xF));
				break;
			case IO_P40:
                P4M1 |= (1 << (pin & 0xF));
                P4M0 |= (1 << (pin & 0xF));
				break;
			case IO_P50:
                P5M1 |= (1 << (pin & 0xF));
                P5M0 |= (1 << (pin & 0xF));
				break;
			case IO_P60:
                P6M1 |= (1 << (pin & 0xF));
                P6M0 |= (1 << (pin & 0xF));
				break;
			case IO_P70:
                P7M1 |= (1 << (pin & 0xF));
                P7M0 |= (1 << (pin & 0xF));
				break;
			default:
				break; // 无效引脚
		}
	}
	else
	{
		// zf_assert(0); // 参数不匹配
	}
}

// 内部使用，用户无需关心。
void gpio_set_level_speed(gpio_pin_enum pin, gpio_speed_enum speed)
{
    // 提前计算引脚位和端口标志
    uint8 pin_bit = pin & 0x0F;
    uint8 port_flag = pin & 0xF0;

    // 根据端口标志操作对应PnSR寄存器（速度配置）
    switch(port_flag)
    {
        case IO_P00:
            // 快速模式：清除位；低速模式：置位
            (GPIO_SPEED_FAST == speed) ? (P0SR &= ~(1 << pin_bit)) : (P0SR |= (1 << pin_bit));
            break;
        case IO_P10:
            (GPIO_SPEED_FAST == speed) ? (P1SR &= ~(1 << pin_bit)) : (P1SR |= (1 << pin_bit));
            break;
        case IO_P20:
            (GPIO_SPEED_FAST == speed) ? (P2SR &= ~(1 << pin_bit)) : (P2SR |= (1 << pin_bit));
            break;
        case IO_P30:
            (GPIO_SPEED_FAST == speed) ? (P3SR &= ~(1 << pin_bit)) : (P3SR |= (1 << pin_bit));
            break;
        case IO_P40:
            (GPIO_SPEED_FAST == speed) ? (P4SR &= ~(1 << pin_bit)) : (P4SR |= (1 << pin_bit));
            break;
        case IO_P50:
            (GPIO_SPEED_FAST == speed) ? (P5SR &= ~(1 << pin_bit)) : (P5SR |= (1 << pin_bit));
            break;
        case IO_P60:
            (GPIO_SPEED_FAST == speed) ? (P6SR &= ~(1 << pin_bit)) : (P6SR |= (1 << pin_bit));
            break;
        case IO_P70:
            (GPIO_SPEED_FAST == speed) ? (P7SR &= ~(1 << pin_bit)) : (P7SR |= (1 << pin_bit));
            break;
        default:
            // zf_assert(0); // 无效引脚
            break;
    }
}


// 内部使用，用户无需关心。
static void gpio_set_dir(gpio_pin_enum pin, gpio_dir_enum dir)
{
    switch(pin & 0xF0)
    {
        case IO_P00:
        {
            if(GPIO == dir || GPI == dir)
            {
                P0M1 &= ~(1 << (pin & 0xF));
                P0M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P0M1 |= (1 << (pin & 0xF));
//                P0M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P0M1 &= ~(1 << (pin & 0xF));
                P0M0 |= (1 << (pin & 0xF));
            }
        }
        break;
        
        case IO_P10:
        {
            if(GPIO == dir || GPI == dir)
            {
                P1M1 &= ~(1 << (pin & 0xF));
                P1M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P1M1 |= (1 << (pin & 0xF));
//                P1M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P1M1 &= ~(1 << (pin & 0xF));
                P1M0 |= (1 << (pin & 0xF));
            }
        }
        break;
        
        case IO_P20:
        {
            if(GPIO == dir || GPI == dir)
            {
                P2M1 &= ~(1 << (pin & 0xF));
                P2M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P2M1 |= (1 << (pin & 0xF));
//                P2M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P2M1 &= ~(1 << (pin & 0xF));
                P2M0 |= (1 << (pin & 0xF));
            }
        }
        break;
        
        case IO_P30:
        {
            if(GPIO == dir || GPI == dir)
            {
                P3M1 &= ~(1 << (pin & 0xF));
                P3M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P3M1 |= (1 << (pin & 0xF));
//                P3M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P3M1 &= ~(1 << (pin & 0xF));
                P3M0 |= (1 << (pin & 0xF));
            }
        }
        break;
        
        case IO_P40:
        {
            if(GPIO == dir || GPI == dir)
            {
                P4M1 &= ~(1 << (pin & 0xF));
                P4M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P4M1 |= (1 << (pin & 0xF));
//                P4M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P4M1 &= ~(1 << (pin & 0xF));
                P4M0 |= (1 << (pin & 0xF));
            }
        }
        break;
        
        case IO_P50:
        {
            if(GPIO == dir || GPI == dir)
            {
                P5M1 &= ~(1 << (pin & 0xF));
                P5M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P5M1 |= (1 << (pin & 0xF));
//                P5M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P5M1 &= ~(1 << (pin & 0xF));
                P5M0 |= (1 << (pin & 0xF));
            }
        }
        break;
		
		case IO_P60:
        {
            if(GPIO == dir || GPI == dir)
            {
                P6M1 &= ~(1 << (pin & 0xF));
                P6M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P6M1 |= (1 << (pin & 0xF));
//                P6M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P6M1 &= ~(1 << (pin & 0xF));
                P6M0 |= (1 << (pin & 0xF));
            }
        }
        break;
		
		case IO_P70:
        {
            if(GPIO == dir || GPI == dir)
            {
                P7M1 &= ~(1 << (pin & 0xF));
                P7M0 &= ~(1 << (pin & 0xF));
            }
//            else if(GPO_OPEN_DTAIN == dir)
//            {
//                P7M1 |= (1 << (pin & 0xF));
//                P7M0 |= (1 << (pin & 0xF));
//            }
            else if(GPO == dir)
            {
                P7M1 &= ~(1 << (pin & 0xF));
                P7M0 |= (1 << (pin & 0xF));
            }

        }
        break;
        
        default:
        {
			zf_assert(0);
			// 没有这个引脚
        } break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPIO状态获取
// 参数说明     pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内gpio_pin_enum枚举值确定)
// 返回参数     uint8       0：低电平 1：高电平
// 使用示例     uint8 status = gpio_get_level(IO_P00);//获取P00引脚电平
//-------------------------------------------------------------------------------------------------------------------
uint8 gpio_get_level(gpio_pin_enum pin)
{
    uint8 pin_bit = pin & 0x0F;
    uint8 port_flag = pin & 0xF0;
    uint8 status = 0;

    // 根据端口标志读取对应Pn寄存器
    switch(port_flag)
    {
        case IO_P00: status = P0 & (1 << pin_bit); break;
        case IO_P10: status = P1 & (1 << pin_bit); break;
        case IO_P20: status = P2 & (1 << pin_bit); break;
        case IO_P30: status = P3 & (1 << pin_bit); break;
        case IO_P40: status = P4 & (1 << pin_bit); break;
        case IO_P50: status = P5 & (1 << pin_bit); break;
        case IO_P60: status = P6 & (1 << pin_bit); break;
        case IO_P70: status = P7 & (1 << pin_bit); break;
        default: break; // 无效引脚
    }
    
    return status ? 1 : 0; // 转换为0/1返回
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPIO输出设置
// 参数说明     pin         引脚号选择的引脚 (可选择范围由 common.h 内GPIO_PIN_enum枚举值确定)
// 参数说明     dat         引脚的电平状态，输出时有效 0：低电平 1：高电平
// 返回参数     void
// 使用示例     gpio_set_level(D0, 0);//D0输出低电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_level(gpio_pin_enum pin, uint8 dat)
{
    uint8 pin_bit = pin & 0x0F;
    uint8 port_flag = pin & 0xF0;

    // 三元运算符精简置位/清零逻辑
    switch(port_flag)
    {
        case IO_P00: dat ? (P0 |= (1 << pin_bit)) : (P0 &= ~(1 << pin_bit)); break;
        case IO_P10: dat ? (P1 |= (1 << pin_bit)) : (P1 &= ~(1 << pin_bit)); break;
        case IO_P20: dat ? (P2 |= (1 << pin_bit)) : (P2 &= ~(1 << pin_bit)); break;
        case IO_P30: dat ? (P3 |= (1 << pin_bit)) : (P3 &= ~(1 << pin_bit)); break;
        case IO_P40: dat ? (P4 |= (1 << pin_bit)) : (P4 &= ~(1 << pin_bit)); break;
        case IO_P50: dat ? (P5 |= (1 << pin_bit)) : (P5 &= ~(1 << pin_bit)); break;
        case IO_P60: dat ? (P6 |= (1 << pin_bit)) : (P6 &= ~(1 << pin_bit)); break;
        case IO_P70: dat ? (P7 |= (1 << pin_bit)) : (P7 &= ~(1 << pin_bit)); break;
        default: break; // 无效引脚
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPIO电平翻转
// 参数说明     pin         引脚号选择的引脚
// 返回参数     void
// 使用示例     gpio_toggle_level(D0);//翻转D0电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle_level(gpio_pin_enum pin)
{
    // 直接调用现有函数，逻辑清晰
    gpio_set_level(pin, !gpio_get_level(pin));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPIO设置高阻输入
// 参数说明     pin         引脚号选择的引脚
// 返回参数     void
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_impedance(gpio_pin_enum pin)
{
    uint8 pin_bit = pin & 0x0F;
    uint8 port_flag = pin & 0xF0;

    // 统一高阻输入配置（PnM1置位，PnM0清零）
    switch(port_flag)
    {
        case IO_P00: P0M1 |= (1 << pin_bit); P0M0 &= ~(1 << pin_bit); break;
        case IO_P10: P1M1 |= (1 << pin_bit); P1M0 &= ~(1 << pin_bit); break;
        case IO_P20: P2M1 |= (1 << pin_bit); P2M0 &= ~(1 << pin_bit); break;
        case IO_P30: P3M1 |= (1 << pin_bit); P3M0 &= ~(1 << pin_bit); break;
        case IO_P40: P4M1 |= (1 << pin_bit); P4M0 &= ~(1 << pin_bit); break;
        case IO_P50: P5M1 |= (1 << pin_bit); P5M0 &= ~(1 << pin_bit); break;
        case IO_P60: P6M1 |= (1 << pin_bit); P6M0 &= ~(1 << pin_bit); break;
        case IO_P70: P7M1 |= (1 << pin_bit); P7M0 &= ~(1 << pin_bit); break;
        default: break; // 无效引脚
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPIO初始化
// 参数说明     pin         选择的引脚
// 参数说明     dir         引脚的方向   输出：GPO   输入：GPI
// 参数说明     dat         引脚初始化时设置的电平状态，输出时有效
// 参数说明     mode        引脚模式（上下拉/开漏等）
// 返回参数     void
// 使用示例     gpio_init(D0, GPO, 1, GPO_PUSH_PULL);//D0初始化为输出、高电平、推挽模式
//-------------------------------------------------------------------------------------------------------------------
void gpio_init(gpio_pin_enum pin, gpio_dir_enum dir, const uint8 dat, gpio_mode_enum mode)
{
	zf_assert(dir == (mode >> 4));

	if(dir == GPIO)
	{
		gpio_set_dir(pin, GPIO);
		gpio_set_mode(pin, mode);
	}
	else if(dir == GPI)
	{
		if(mode == GPI_IMPEDANCE)
		{
			gpio_set_impedance(pin);
		}
		else
		{
			gpio_set_dir(pin, GPI);
			gpio_set_mode(pin, mode);
		}
	}
	else if(dir == GPO)
	{
		if(GPO_PUSH_PULL == mode)
		{
			gpio_set_dir(pin, GPO);
			gpio_set_mode(pin, GPI_PULL_UP);
		}
		else if(GPO_OPEN_DTAIN == mode)
		{
			gpio_set_mode(pin, GPO_OPEN_DTAIN);
		}
	}
	gpio_set_level(pin, dat);
}
