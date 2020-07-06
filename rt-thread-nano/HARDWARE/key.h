
#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

#define KEY_NUM   4
#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 


#define KEY0_PRES 	1	//KEY0按下
#define KEY1_PRES	2	//KEY1按下
#define KEY2_PRES	4	//KEY2按下
#define WKUP_PRES   8	//KEY_UP按下(即WK_UP/KEY_UP)
 
void KEY_Init(void);
u32 KEY_Read(void);


#endif

