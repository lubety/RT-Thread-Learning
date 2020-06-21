#ifndef __BOARD_H__
#define __BOARD_H__


#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "key.h"


void rt_hw_board_init(void);
void SysTick_Handler(void);






#endif
