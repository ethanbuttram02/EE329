/*
 * keypad.h
 *
 *  Created on: Apr 16, 2024
 *      Author: Seth Saxena
 */
#include "stm32l4xx_hal.h"

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#define COL_PORT (GPIOD)
#define ROW_PORT (GPIOD)
#define COL_PINS (0x00F0)
#define ROW_PINS (0x000F)
#define NUM_ROWS (4)
#define NUM_COLS (4)
#define SETTLE (8000)
#define BIT0 (1)
#define NO_KEYPRESS (-1)
#define TRUE (1)
#define FALSE (0)

void Keypad_Config(void);
int Keypad_IsAnyKeyPressed(void);
int Keypad_WhichKeyIsPressed(void);
int Debounce(int state);

#endif /* INC_KEYPAD_H_ */
