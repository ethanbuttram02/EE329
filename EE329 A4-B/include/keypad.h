/*
 * keypad.h
 *
 *  Created on: Apr 15, 2024
 *      Author: eugie
 * Wire up lowest 4 bits of the A port
 * to pins corresponding to columns
 * Wire up lowest 4 bits of the F port
 * to pins corresponding to rows
 *
 */
#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include "stm32l4xx_hal.h"

#define COL_PORT GPIOA
#define ROW_PORT GPIOF

//Prototype Functions
void Keypad_Config(void);
uint16_t Keypad_IsAnyKeyPressed(void);
uint16_t Keypad_WhichKeyIsPressed(void);
void Keypad_debounce(uint8_t key);
uint8_t Keypad_star_press(void);

//Defines Column and Row Pins
#define COL_PINS 0x0F
#define ROW_PINS 0x0F

//Defines zero key to be 11
#define KEY_ZERO 11
#define NO_KEYPRESS 13

#endif /* INC_KEYPAD_H_ */

