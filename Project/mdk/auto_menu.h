#ifndef _AUTO_MENU_H
#define _AUTO_MENU_H

#include "zf_common_headfile.h"
#include "zf_device_ips114.h"

//-------------------------------------------------------------------------------------------------------------------
// 调试配置宏
//-------------------------------------------------------------------------------------------------------------------
//#define DEBUG_PRINTF                  // 定义此宏开启调试打印
#define _pid  struct controller

#ifdef DEBUG_PRINTF
    #define debug_printf(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define debug_printf(format, ...)
#endif

//-------------------------------------------------------------------------------------------------------------------
// FLASH定义宏
//-------------------------------------------------------------------------------------------------------------------
#ifdef FLASH_PAGE_LENGTH
    #define EEPROM_PAGE_LENGTH FLASH_PAGE_LENGTH
#endif

//-------------------------------------------------------------------------------------------------------------------
// 菜单布局与显示配置宏
//-------------------------------------------------------------------------------------------------------------------
#define MAP_INIT_BEGIN      (0x01<<0)
#define MAP_INIT_END        (0x01<<1)

// IPS114 屏幕高度为 135，字体高度 16，135/16≈8.4，所以每页最多显示 8 行
#define SON_NUM             (8)          // 每页显示的子项数量
#define STR_LEN_MAX         32          // 菜单名称最大字符长度
#define MOUSE_DIS           10          // 光标与文字的X轴间距
#define MOUSE_LOOK          ">"         // 光标样式字符
#define BEGIN_BGCOLOR       BLACK       // 默认背景颜色
#define PID_RING_LEN        600         // PID波形缓冲区长度

//-------------------------------------------------------------------------------------------------------------------
// 链表索引计算宏 (核心逻辑，勿动)
//-------------------------------------------------------------------------------------------------------------------
#define IND1                                (P_dad_head->down->m_index[0])
#define IND2                                (P_dad_head->down->enter->down->m_index[1])
#define SON_END_UNIT                        (P_dad_head->down->enter->down)
#define SON_BEGIN_UNIT                      (P_dad_head->down->enter)

#define DAD_INDEX(_ind1,_ind2)              (*(p_index_xy_dad+_ind1+DAD_NUM*_ind2))
#define SON_INDEX(_ind1,_ind2)              (*(p_index_xy_son+_ind1+SON_NUM*_ind2))

#define IS_PRESS                            (button1||button2||button3||button4)
#define IS_OK                               (button2==1)&&(!first_in_page_flag)

#define CLEAR_PID_DRAW()                    // 留空以防报错

//-------------------------------------------------------------------------------------------------------------------
// 枚举定义
//-------------------------------------------------------------------------------------------------------------------
typedef enum TYPE_VALUE
{
    TYPE_FLOAT=1,       
    TYPE_DOUBLE,        
    TYPE_INT,           
    TYPE_UINT16,        
    TYPE_UINT32,        
    TYPE_BOOL           
} type_value;

typedef enum UNIT_TYPE
{
    USE_FUN=1,          
    NORMAL_PAR,         
    PID_PAR,            
    READ_ONLY_PAR       
} unit_type;

//-------------------------------------------------------------------------------------------------------------------
// 硬件抽象层 (HAL) - 屏幕驱动映射
//-------------------------------------------------------------------------------------------------------------------
#define TFT                     0
#define IPS114                  1       
#define SCREEN_TYPE             IPS114  

#if SCREEN_TYPE==TFT
    #define         screen_init             lcd_init
    #define         clear                   lcd_clear
    #define         showfloat               lcd_showfloat
    #define         showimage032            lcd_displayimage032
    #define         showstr                 lcd_showstr
    #define         showint32               lcd_showint32
    #define         showuint16              lcd_showuint16
    #define         showuint16              lcd_showuint16
    #define         DIS_X                   TFT_X_MAX/2
    #define         DIS_Y                   1
    #define         SCREEN_W                TFT_X_MAX
    #define         SCREEN_H                TFT_Y_MAX
#else
    #define         IPS114_W                240
    #define         IPS114_H                135

    #define         screen_init             ips114_init
    #define         clear                   ips114_clear
    #define         showstr                 ips114_show_string
    #define         drawpoint               ips114_draw_point

    #define         DIS_X                   (IPS114_W/2) 
    #define         DIS_Y                   16           
    #define         SCREEN_W                IPS114_W
    #define         SCREEN_H                IPS114_H
#endif

//-------------------------------------------------------------------------------------------------------------------
// 核心结构体定义
//-------------------------------------------------------------------------------------------------------------------
typedef struct PARAM_SET
{
    void* p_par;          
    float               delta;          
    type_value          par_type;       
    uint8               num;            
    uint8               point_num;      
} param_set;

typedef struct MENU_UNIT
{
    param_set* par_set;        

    struct MENU_UNIT* up;             
    struct MENU_UNIT* down;           
    struct MENU_UNIT* enter;          
    struct MENU_UNIT* back;           

    void (*current_operation)(void);    

    char  name[STR_LEN_MAX];            
    uint8 m_index[2];                   
    unit_type           type_t;         
} menu_unit;

// =========================================================================
// 【关键修改点】缩小内存池
// 原来的 100 会占用超过 7KB 内存，导致 STC32G 编译报 L256 UNKNOWN 错误。
// 这里修改为 25，足够你存 20 多个变量，同时把内存占用降到 1.5KB 左右。
// =========================================================================
#define USE_STATIC_MENU
#ifdef USE_STATIC_MENU
#define MEM_SIZE    25                 
#endif

//-------------------------------------------------------------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------------------------------------------------------------
extern uint8 is_editing_mode;           
extern int duty;                        

extern int motor_select;
extern int motor_test_duty;

//-------------------------------------------------------------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------------------------------------------------------------
void menu_init(void);
void show_process(void *parameter);
void UNIT_SET(void);
void FUN_INIT(void);
void unit_param_set(void* p_param, type_value t, float delta, uint8 num, uint8 point_num, unit_type t1, const char _name[STR_LEN_MAX]);
void fun_init(void (*fun)(void), const char* _name);
//void Flash_Save(void);
//void Flash_Read(void);
void show_menu(void);
void center_menu(void);
void assist_menu(void);
void get_node_string(menu_unit* p, char* buffer);
uint8 is_menu_clear(void);
void is_first_in_page(void);
void change_value_step(param_set* param, int dir);
void show_camera_window(void);
void index_xy_init(void);
void dad_name_init(void);
void son_link(menu_unit *p_1,menu_unit *p_2,menu_unit *dad);
void dad_link(menu_unit *p_1,menu_unit *p_2,menu_unit *son);
void unit_default(menu_unit *p,uint8 ind);
void unit_index_init(menu_unit *_p1,uint8 ind_0,uint8 ind_1);
void NULL_FUN(void);

#endif