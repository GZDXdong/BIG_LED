#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f4xx_hal.h"
#include "sys_macro.h"	  

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);
#endif

