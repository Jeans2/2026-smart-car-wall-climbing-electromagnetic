#ifndef __XUNJI_H
#define __XUNJI_H

       
#include "bsp_system.h"
extern int8 element;
extern float distance_ringR;










void xunji(void);
void ringR_task(void);
void ringR_execute(void);
void ringL_task(void);
void ringL_execute(void);

#endif