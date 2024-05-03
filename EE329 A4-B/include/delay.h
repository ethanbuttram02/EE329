/*
 * delay.h
 *
 *  Created on: Apr 23, 2024
 *      Author: eugie
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32l4xx_hal.h"

void delay_us(const uint32_t time_us);
void SysTick_Init(void);

#endif /* INC_DELAY_H_ */
