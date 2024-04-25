/*
 * delay.h
 *
 *  Created on: Apr 24, 2024
 *      Author: Ethan Buttram
 */

#include "stm32l4xx_hal.h"

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
