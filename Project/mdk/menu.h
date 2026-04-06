#ifndef __MENU_H
#define __MENU_H


#include "bsp_system.h"
void UI_Display_Update(void);
void Key_Menu_Adjust(void);
extern uint8 start_ramp_flag;
void Save_Params_To_EEPROM(void);
void Load_Params_From_EEPROM(void);




















#endif