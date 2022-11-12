#ifndef __TIMER_H
#define __TIMER_H

#define servo(num) TIM_SetCompare1(TIM1,num);
#include "stm32f10x.h"                  // Device header

void Timer1_Pwm_Init(void);
void Timer2_Pwm_Init(u16 arr,u16 psc);

#endif
