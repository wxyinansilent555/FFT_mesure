#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "sys.h"

#define servo(num) TIM_SetCompare4(TIM1,num)

void TIM1_PWM_Init(void);
void TIM2_PWM_Init(u16 arr,u16 psc);

#endif
