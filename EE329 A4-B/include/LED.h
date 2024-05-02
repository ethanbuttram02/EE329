/*
 * LED.h
 *
 *  Created on: Apr 16, 2024
 *      Author: eugie
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32l4xx_hal.h"

typedef int32_t var_type;

//Prototype Functions
void Delay(uint32_t time);
void LED_Config(void);
void LED_Clock(void);
var_type TestFunction(var_type num);

#endif /* INC_LED_H_ */
