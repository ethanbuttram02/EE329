/*
 * LCD.h
 *
 *  Created on: Apr 22, 2024
 *      Author: Eugenio Pasos
 *
 * Wire up the lowest 4 bits of the D port
 * to the highest 4 bits of the LCD data port
 * Wire pin 4 to Register set bit of LCD
 * Wire pin 5 to Enable bit of LCD
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "stm32l4xx_hal.h"
#include "delay.h"
#include "keypad.h"
#include "led.h"

//Defining LCD bits
#define LCD_PORT GPIOD
#define LCD_DATA_BITS 0x0F
#define LCD_EN 0x20
#define LCD_RS 0x10

//Prototype Functions
void LCD_Config(void);
void LCD_init(void);
void LCD_pulse_ENA(void);
void LCD_4b_command(uint8_t command);
void LCD_command(uint8_t command);
void LCD_write_char(uint8_t letter);
void LCD_write_string(char *word);
void LCD_setup_counter(void);
void LCD_count_start(char min, char sec);

#endif /* INC_LCD_H_ */
