/* USER CODE BEGIN Header */
   /****************************************************************************
   * EE 329 A3 Parallel LCD
   *****************************************************************************
   * @file           : lcd.h
   * @brief          : Header for lcd.c
   * project         : EE 329 - Spring 2024 - A2
   * authors         : Michael Mazzola - mfmazzol@calpoly.edu
   *                 : Katelynn Hoang - khoang19@calpoly.edu
   *                 : Ethan Buttram - ebuttram@calpoly.edu
   * version         : 0.1
   * date            : 04-24-2024
   * compiler        : GNU Tools for STM32 (12.3rel1)
   * target          : NUCLEO-L4A6ZG
   * clocks          : 4MHz MSI to AHB2
   * @Description : Function prototypes, includes, #defines, etc. for keypad.c
   *
   *****************************************************************************
   * @attention
   *
   * Copyright (c) 2024 STMicroelectronics.
   * All rights reserved.
   *
   * This software is licensed under terms that can be found in the LICENSE file
   * in the root directory of this software component.
   * If no LICENSE file comes with this software, it is provided AS-IS.
   *
   ****************************************************************************/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

// Define constant values
#define CLEAR 0b00000001
#define NEW_LINE 0b11000000
#define LCD_PORT GPIOC          // set port A pins for the LCD
#define LCD_DATA_BITS 0xF       // Data bits are set as pins 0,1,2,3    PC0: CN10 - 29, PC1: CN10 - 11, PC2: CN10 - 13, PC3: CN9 - 1
#define LCD_EN GPIO_PIN_4       // Enable bit is set as pin 4           PC4: CN7 - 17
#define LCD_RW GPIO_PIN_5       // Read/Write bit set as pin 5          PC5: CN7 - 10
#define LCD_RS GPIO_PIN_6       // Register Select bit is set as pin 6  PC6: CN7 - 12
#define LCD_COUNT_ADR 0xCB      // Command for setting address to count start
#define LCD_SET_DDRAM_ADDR 0x80 // Command to set ddram address

void LCD_init( void );
void LCD_pulse_ENA( void);
void LCD_4b_command(uint8_t command );
void LCD_command( uint8_t command );
void LCD_write_char( uint8_t letter );
void LCD_write_string( const char str[] );
void LCD_update_display(int minutes, int seconds);
void LCD_set_cursor(uint8_t row, uint8_t col);


#ifdef __cplusplus
extern "C" {
#endif


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);



#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
