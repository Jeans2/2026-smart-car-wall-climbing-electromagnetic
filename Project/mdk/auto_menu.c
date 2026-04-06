#include "auto_menu.h"

/*===================================================================================================================
                                    智能车多级菜单与参数系统使用说明书
=====================================================================================================================

一、 按键操作说明 (Key Mapping)
---------------------------------------------------------------------------------------------------------------------
    Button 1 (P20_6): [返回/取消]  退出当前菜单，或退出编辑模式。
    Button 2 (P20_7): [确认/进入]  进入子菜单，或进入参数编辑模式(光标变*)，或执行功能函数。
    Button 3 (P11_2): [上翻/增加]  向上移动光标，或增加参数数值。
    Button 4 (P11_3): [下翻/减少]  向下移动光标，或减少参数数值。
    需要修改按键引脚请在key.h中修改

二、 核心功能说明
---------------------------------------------------------------------------------------------------------------------
    1. [普通参数] (NORMAL_PAR): 可以查看，按下确认键后可修改数值。
    2. [只读参数] (READ_ONLY_PAR): 仅用于显示传感器数据(如电感、陀螺仪)，不可修改。
    3. [功能函数] (USE_FUN): 按下确认键后执行特定函数(如看图像)。

===================================================================================================================*/

uint8 button1=0,button2=0,button3=0,button4=0;

// 页面状态标志位
uint8 first_in_page_flag = 0;   // 首次进入页面标志
uint8 is_clear_flag=0;          // 清屏标志

// 坐标索引指针
uint8* p_index_xy_dad,*p_index_xy_son;

// 菜单节点指针
static menu_unit* p_unit      = NULL; // 当前选中的菜单单元
static menu_unit* p_unit_last = NULL; // 上一次选中的菜单单元
static menu_unit* P_dad_head  = NULL; // 父菜单链表头指针

// 父菜单页数计数
uint8 DAD_NUM=1;

// 静态内存池分配
menu_unit   my_menu_unit[MEM_SIZE];
param_set   my_param_set[MEM_SIZE];
uint8       my_index[MEM_SIZE*2];
static int  static_cnt=0;

// 全局状态机：0=浏览模式(导航), 1=编辑模式(修改数值)
uint8 is_editing_mode = 0;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      辅助菜单显示函数
//-------------------------------------------------------------------------------------------------------------------
void assist_menu()
{
    char display_buf[50]; // 用于存放临时生成的字符串
    menu_unit* p_start;
    menu_unit* p_curr;
    uint8 i;
    uint16 x, y;

    // 1. 找到当前页的起始节点
    p_start = p_unit->back->enter;
    p_curr = p_start;

    // 2. 循环遍历本页所有节点
    for(i=0; i<SON_NUM; i++)
    {
        get_node_string(p_curr, display_buf);

        x = SON_INDEX(p_curr->m_index[1], 0) + MOUSE_DIS;
        y = SON_INDEX(p_curr->m_index[1], 1);

        showstr(x, y, display_buf);

        // 5. 绘制光标逻辑
        if(p_curr == p_unit)
        {
            if(is_editing_mode)
                showstr(x - MOUSE_DIS, y, "*"); // 编辑模式显示 *
            else
                showstr(x - MOUSE_DIS, y, ">"); // 浏览模式显示 >
        }
        else
        {
            showstr(x - MOUSE_DIS, y, " ");
        }

        // 6. 移动到下一个节点
        p_curr = p_curr->down;

        // 7. 特殊处理：如果不满一页，则提前退出循环
        if(p_curr == p_start && i != SON_NUM - 1) break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      获取菜单节点显示字符串
//-------------------------------------------------------------------------------------------------------------------
void get_node_string(menu_unit* p, char* buffer)
{
    void* val;
    float f_val;
    int i_part, d_part;
    double d_val;

    if(p->type_t == USE_FUN)
    {
        sprintf(buffer, "%s", p->name);
        return;
    }

    val = p->par_set->p_par;
    if(val == NULL)
    {
        sprintf(buffer, "%s:NULL", p->name);
        return;
    }

    switch(p->par_set->par_type)
    {
        case TYPE_FLOAT:
            f_val = *(float*)val;
            i_part = (int)f_val;
            d_part = (int)((f_val - i_part) * 100); 
            if(d_part < 0) d_part = -d_part; 
            sprintf(buffer, "%s:%d.%02d  ", p->name, i_part, d_part); 
            break;

        case TYPE_DOUBLE:
            d_val = *(double*)val;
            i_part = (int)d_val;
            d_part = (int)((d_val - i_part) * 100);
            if(d_part < 0) d_part = -d_part;
            sprintf(buffer, "%s:%d.%02d  ", p->name, i_part, d_part);
            break;

        case TYPE_INT:
            sprintf(buffer, "%s:%d  ", p->name, *(int*)val);
            break;

        case TYPE_UINT16:
            sprintf(buffer, "%s:%u  ", p->name, *(uint16*)val);
            break;

        case TYPE_UINT32:
            sprintf(buffer, "%s:%u  ", p->name, (unsigned int)*(uint32*)val);
            break;

        case TYPE_BOOL:
             sprintf(buffer, "%s:%s  ", p->name, (*(uint8*)val)?"ON ":"OFF");
             break;

        default:
            sprintf(buffer, "%s:Err  ", p->name);
            break;
    }
}

// 函数指针变量
void (*current_operation_menu)(void);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      父菜单名称初始化
//-------------------------------------------------------------------------------------------------------------------
void dad_name_init()
{
    char p[STR_LEN_MAX];
    uint8 i;

    memset(p, 0, STR_LEN_MAX);
    strcpy(p, "Page ");

    DAD_NUM = IND1 + 1;
    for(i=0; i<DAD_NUM; i++)
    {
        if(i<10)
        {
            p[5] = '0'+ i;
            strcpy(P_dad_head->name, p);
        }
        else
        {
            p[5] = '0'+ i/10;
            p[6] = '0'+ i%10;
            strcpy(P_dad_head->name, p);
        }
        P_dad_head = P_dad_head->up;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      构建子菜单链表连接
//-------------------------------------------------------------------------------------------------------------------
void son_link(menu_unit *p_1,menu_unit *p_2,menu_unit *dad)
{
        p_1->up     =   p_2;
        p_2->down   =   p_1;
        p_1->enter  =   p_1;
        p_2->enter  =   p_2;
        p_1->back   =   dad;
        p_2->back   =   dad;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      构建父菜单链表连接
//-------------------------------------------------------------------------------------------------------------------
void dad_link(menu_unit *p_1,menu_unit *p_2,menu_unit *son)
{
    p_1-> up    = p_2;
    p_2-> down  = p_1;
    p_1-> back  = p_1;
    p_2-> back  = p_2;
    if(son!=NULL)
        p_1->enter  =   son;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      菜单单元默认初始化
//-------------------------------------------------------------------------------------------------------------------
void unit_default(menu_unit *p,uint8 ind)
{
    p->type_t = USE_FUN;
    p->current_operation = NULL_FUN;
    p->m_index[0]=ind;
    p->m_index[1]=255;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      菜单索引初始化
//-------------------------------------------------------------------------------------------------------------------
void unit_index_init(menu_unit *_p1,uint8 ind_0,uint8 ind_1)
{
    _p1->m_index[0]=ind_0;
    _p1->m_index[1]=ind_1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      配置一个参数菜单项
//-------------------------------------------------------------------------------------------------------------------
void unit_param_set(void* p_param, type_value t, float delta, uint8 num, uint8 point_num, unit_type t1, const char _name[STR_LEN_MAX])
{
    menu_unit *p1 = NULL;
    param_set *p1_par = NULL;
    static menu_unit *dad;

    p1 = my_menu_unit + static_cnt;
    p1_par = my_param_set + static_cnt;
    static_cnt++;

    if(P_dad_head == NULL)
    {
        dad = my_menu_unit + static_cnt;
        static_cnt++;
        P_dad_head = dad;
        unit_default(dad, 0);
        p_unit = dad;

        unit_index_init(p1, 0, 0);
        dad_link(dad, dad, p1);
        son_link(p1, p1, dad); 
    }
    else
    {
        if(IND2 >= SON_NUM - 1)
        {
            dad = my_menu_unit + static_cnt;
            static_cnt++;
            unit_default(dad, IND1 + 1);
            unit_index_init(p1, IND1 + 1, 0);

            dad_link(P_dad_head->down, dad, NULL);
            dad_link(dad, P_dad_head, p1);
            son_link(p1, p1, dad);
        }
        else
        {
            unit_index_init(p1, IND1, IND2 + 1);
            son_link(SON_END_UNIT, p1, P_dad_head->down);
            son_link(p1, SON_BEGIN_UNIT, P_dad_head->down);
        }
    }

    p1->par_set = p1_par;
    p1->par_set->p_par = p_param;
    p1->par_set->par_type = t;
    p1->par_set->delta = delta;
    p1->par_set->num = num;
    p1->par_set->point_num = point_num;
    p1->type_t = t1; 

    memset(p1->name, 0, STR_LEN_MAX);
    strcpy(p1->name, _name);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      配置一个函数菜单项
//-------------------------------------------------------------------------------------------------------------------
void fun_init(void (*fun)(void), const char* _name)
{
    menu_unit *p2 = NULL;
    menu_unit *p1 = NULL;
    uint8 i;

    p1 = my_menu_unit+static_cnt;
    static_cnt++;
    p1->par_set = NULL;

    if(IND2>=SON_NUM-1)
    {
        p2 = my_menu_unit+static_cnt;
        static_cnt++;
        unit_default(p2,IND1+1);
        unit_index_init(p1,IND1+1,0);
        dad_link(P_dad_head->down,p2,NULL);
        dad_link(p2,P_dad_head,p1);
        son_link(p1,p1,p2);
    }
    else
    {
        unit_index_init(p1,IND1,IND2+1);
        son_link(SON_END_UNIT,p1,P_dad_head->down);
        son_link(p1,SON_BEGIN_UNIT,P_dad_head->down);
    }

    p1->current_operation = fun;
    for(i=0;i<STR_LEN_MAX;i++)p1->name[i]=0;
    strcpy(p1->name, _name);
    p1->type_t = USE_FUN;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      初始化显示坐标索引表
//-------------------------------------------------------------------------------------------------------------------
void index_xy_init()
{
    uint8 half;
    uint8 i;

    p_index_xy_dad = my_index;
    p_index_xy_son = my_index+DAD_NUM*2;
    
    half = (DAD_NUM+1)/2;

    for(i=0;i<half;i++)
    {
        DAD_INDEX(i,0)=DIS_X*0;
        DAD_INDEX(i,1)=DIS_Y*i;
    }
    for(i=half;i<DAD_NUM;i++)
    {
        DAD_INDEX(i,0)=DIS_X*1;
        DAD_INDEX(i,1)=DIS_Y*(i-half);
    }

    for(i=0;i<SON_NUM;i++)
    {
        SON_INDEX(i,0)=DIS_X*0;
        SON_INDEX(i,1)=DIS_Y*i;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      检测是否需要清屏
//-------------------------------------------------------------------------------------------------------------------
uint8 is_menu_clear()
{
    return (((p_unit->back)!=(p_unit))&&button1)||(((p_unit->enter)!=(p_unit))&&button2);
}

uint8 begin_menu_flag=1;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      父级菜单显示函数
//-------------------------------------------------------------------------------------------------------------------
void center_menu()
{
    uint8 index;
    menu_unit* p;
    uint8 i;

    index = p_unit->m_index[0];
    p = p_unit;

    if(first_in_page_flag)
        showstr(DAD_INDEX(index,0),DAD_INDEX(index,1),MOUSE_LOOK); 

    if(button3||button4)
    {
        if(index==0)
        {
            showstr(DAD_INDEX(DAD_NUM-1,0),DAD_INDEX(DAD_NUM-1,1)," ");
            showstr(DAD_INDEX(0,0) ,DAD_INDEX(0,1),MOUSE_LOOK);
            showstr(DAD_INDEX(1,0) ,DAD_INDEX(1,1)," ");
        }
        else if(index==DAD_NUM-1)
        {
            showstr(DAD_INDEX(DAD_NUM-2,0),DAD_INDEX(DAD_NUM-2,1)," ");
            showstr(DAD_INDEX(DAD_NUM-1,0),DAD_INDEX(DAD_NUM-1,1),MOUSE_LOOK);
            showstr(DAD_INDEX(0,0) ,DAD_INDEX(0,1) ," ");
        }
        else
        {
            showstr(DAD_INDEX(index-1,0),DAD_INDEX(index-1,1)," ");
            showstr(DAD_INDEX(index  ,0),DAD_INDEX(index  ,1),MOUSE_LOOK);
            showstr(DAD_INDEX(index+1,0),DAD_INDEX(index+1,1)," ");
        }
    }
    else if((is_clear_flag==1&&(button1))||(begin_menu_flag))
    {
        for(i=0;i<DAD_NUM;i++)
        {
            showstr(DAD_INDEX(p->m_index[0],0)+MOUSE_DIS,DAD_INDEX(p->m_index[0],1),p->name);
            p = p->up;
        }
        begin_menu_flag = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      参数数值修改核心函数
//-------------------------------------------------------------------------------------------------------------------
void change_value_step(param_set* param, int dir)
{
    float delta;

    if(param == NULL || param->p_par == NULL) return;

    delta = param->delta;
    if(dir < 0) delta = -delta;

    switch(param->par_type)
    {
        case TYPE_FLOAT:
            *(float*)(param->p_par) += delta;
            break;
        case TYPE_DOUBLE:
            *(double*)(param->p_par) += (double)delta;
            break;
        case TYPE_INT:
            *(int*)(param->p_par) += (int)delta;
            break;
        case TYPE_UINT16:
            if(delta < 0 && *(uint16*)(param->p_par) < (uint16)(-delta))
                *(uint16*)(param->p_par) = 0;
            else
                *(uint16*)(param->p_par) += (int)delta;
            break;
        case TYPE_UINT32:
            if(delta < 0 && *(uint32*)(param->p_par) < (uint32)(-delta))
                *(uint32*)(param->p_par) = 0;
            else
                *(uint32*)(param->p_par) += (int)delta;
            break;
        case TYPE_BOOL:
            *(uint8*)(param->p_par) = !(*(uint8*)(param->p_par));
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      检测是否首次进入新页面
//-------------------------------------------------------------------------------------------------------------------
void is_first_in_page()
{
    first_in_page_flag = (p_unit_last!=p_unit)&&(button1||button2);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      主菜单显示入口
//-------------------------------------------------------------------------------------------------------------------
void show_menu()
{
    if(p_unit->m_index[1]==255)
        center_menu(); // 显示父菜单
    else
        assist_menu(); // 显示子菜单
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      菜单逻辑处理主循环
//-------------------------------------------------------------------------------------------------------------------
void show_process(void *parameter)
{
//    (void)parameter; // 消除未调用警告

    // 1. 获取按键 (此处保留原来的调用方式)
    button_entry(NULL);

    // 2. 检测是否需要清屏
    is_clear_flag = is_menu_clear();
    if(is_clear_flag)
    {
        clear(0x0000); // 0x0000 为黑色，防止原宏找不到报错
        is_editing_mode = 0; 
    }

    // 3. 核心状态机逻辑
    if (is_editing_mode == 0)
    {
        if (button1 == 1)
        {
            p_unit = p_unit->back;
        }
        else if (button3 == 1)
        {
            p_unit = p_unit->up;
        }
        else if (button4 == 1)
        {
            p_unit = p_unit->down;
        }
        else if (button2 == 1) 
        {
            if(p_unit->m_index[1] == 255)
            {
                if(p_unit->enter != p_unit)
                {
                    p_unit = p_unit->enter;
                }
            }
            else
            {
                if (p_unit->type_t == USE_FUN)
                {
                    if(p_unit->current_operation) (*p_unit->current_operation)();
                }
                else if (p_unit->type_t == READ_ONLY_PAR)
                {
                    // 只读参数无操作
                }
                else
                {
                    is_editing_mode = 1;
                }
            }
        }
    }
    else
    {
        if (button1 == 1 || button2 == 1)
        {
            is_editing_mode = 0;
        }
        else if (button3 == 1)
        {
            change_value_step(p_unit->par_set, 1);
        }
        else if (button4 == 1)
        {
            change_value_step(p_unit->par_set, -1);
        }
    }

    is_first_in_page();

    // 4. 显示刷新
    show_menu();

    // 5. 记录历史
    p_unit_last = p_unit;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      空函数占位符
//-------------------------------------------------------------------------------------------------------------------
void NULL_FUN()
{
}

// 示例变量
float test_a=1.1;
int test_b=100;
double test_c=100;
uint16 test_d=20;
uint32 test_e=32;
uint8 flag = 0;
uint32 test_f=32;
uint32 test_g=32;
uint32 test_h=32;
uint32 test_i=32;
int duty;
int motor_select = 0;
int motor_test_duty = 0;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      用户参数配置初始化
//-------------------------------------------------------------------------------------------------------------------
void UNIT_SET()
{
    unit_param_set(&test_a, TYPE_FLOAT,  0.5,    0, 0, NORMAL_PAR,    "test_a");
    unit_param_set(&test_b, TYPE_INT,    2,      0, 0, NORMAL_PAR,    "test_b");
    unit_param_set(&test_c, TYPE_DOUBLE, 10.01,  0, 0, NORMAL_PAR,    "test_c");
    unit_param_set(&test_d, TYPE_UINT16, 1,      0, 0, NORMAL_PAR,    "test_d");
    unit_param_set(&test_e, TYPE_UINT32, 1,      0, 0, NORMAL_PAR,    "test_e");
    unit_param_set(&flag,   TYPE_BOOL,   0,      0, 0, NORMAL_PAR,    "flag");
    unit_param_set(&test_f, TYPE_UINT32, 1,      0, 0, NORMAL_PAR,    "test_f");
    unit_param_set(&test_g, TYPE_UINT32, 1,      0, 0, NORMAL_PAR,    "test_g");
    unit_param_set(&test_h, TYPE_UINT32, 1,      0, 0, NORMAL_PAR,    "test_h");
    unit_param_set(&test_i, TYPE_UINT32, 1,      0, 0, NORMAL_PAR,    "test_i");
    unit_param_set(&duty,   TYPE_INT,    0,      0, 0, READ_ONLY_PAR, "duty");

    unit_param_set(&motor_select, TYPE_INT, 1, 0, 0, NORMAL_PAR, "MotorSel(0-4)");
    unit_param_set(&motor_test_duty, TYPE_INT, 100, 0, 0, NORMAL_PAR, "TestSpeed");
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      用户功能函数初始化
//-------------------------------------------------------------------------------------------------------------------
void FUN_INIT()
{
    fun_init(show_camera_window, "Camera View");
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      菜单系统整体初始化
//-------------------------------------------------------------------------------------------------------------------
void menu_init()
{
    /*---------------屏幕初始化-----------------*/
    screen_init(); // 调用统一宏，由于头文件映射成了ips114_init()，屏幕会正常启动

    /*---------------按键初始化-----------------*/
    // key_into(); // 此处注释掉或保留，看你是否有key.h的底层支持

    /*---------------待修改参数-----------------*/
    UNIT_SET();

    /*---------------导入的功能函数--------------*/
    FUN_INIT();

    /*---------------菜单名字初始化--------------*/
    dad_name_init();

    /*---------------字符串索引初始化-------------*/
    index_xy_init();
}