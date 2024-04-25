/*
 * LCD.h
 *
 *  Created on: Apr 23, 2024
 *      Author: seth saxena and Ethan Buttram
 */
#include "stm32l4xx_hal.h"

#ifndef INC_LCD_H_
#define INC_LCD_H_

#define LCD_PORT GPIOC
#define LCD_EN GPIO_PIN_8 // ENABLE is pin 8
#define LCD_DATA_BITS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LCD_RS GPIO_PIN_9  // RESET is pin 9
#define LCD_RW GPIO_PIN_10 // READ/WRITE is pin 10

void LCD_config(void);
void LCD_init(void);
void LCD_pulse_ENA(void);
void LCD_4b_command(uint8_t command);
void LCD_command(uint8_t command);
void LCD_write_char(uint8_t letter);

#endif /* INC_LCD_H_ */
