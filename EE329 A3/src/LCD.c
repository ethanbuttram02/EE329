/*
 * LCD.c
 *
 *  Created on: Apr 23, 2024
 *      Author: sethsaxena and ethan buttram
 */

#include "LCD.h"
#include "stm32l4xx_hal.h"
#include "delay.h"
#include <math.h>

// 4-bit Initialization:
//  Sample code that was edited from the LCD data sheet and Lab Manual
void LCD_config(void)
{
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN); // enables GPIO clock

	LCD_PORT->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
						 GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
						 GPIO_MODER_MODE8 | GPIO_MODER_MODE9 |
						 GPIO_MODER_MODE10);
	// sets PC0-PC3 to general-purpose output mode
	LCD_PORT->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 |
						GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
						GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 |
						GPIO_MODER_MODE10_0);
	// sets pins to push-pull
	LCD_PORT->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 |
						  GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
						  GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9 |
						  GPIO_OTYPER_OT10);
	// disables PU and PD resistors for LCD pins
	LCD_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
						 GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
						 GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9 |
						 GPIO_PUPDR_PUPD10);
	// sets speed for PD4-PD7 to high
	LCD_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED3_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED8_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED9_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED10_Pos));
}

void LCD_init(void)
{
	delay_us(40000); // power-up wait 40 ms
	for (int idx = 0; idx < 3; idx++)
	{						  // wake up 1,2,3: DATA = 0011 XXXX
		LCD_4b_command(0x30); // HI 4b of 8b cmd, low nibble = X
		delay_us(200);
	}
	LCD_4b_command(0x20); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
	delay_us(40);
	LCD_command(0x28); // Function set: 4-bit/2-line
	delay_us(40);
	LCD_command(0x10); // Set cursor
	delay_us(40);
	LCD_command(0x0F); // Display ON; Blinking cursor
	delay_us(40);
	LCD_command(0x06); // Entry Mode set
	delay_us(40);
	LCD_command(0x01); // clr LCD
}

void LCD_pulse_ENA(void)
{
	// ENAble line sends command on falling edge
	// set to restore default then clear to trigger
	LCD_PORT->ODR |= (LCD_EN);	// ENABLE = HI
	delay_us(500);				// TDDR > 320 ns
	LCD_PORT->ODR &= ~(LCD_EN); // ENABLE = LOW
	delay_us(500);				// low values flakey, see A3:p.1
}

void LCD_4b_command(uint8_t command)
{
	// LCD command using high nibble only - used for 'wake-up' 0x30 commands
	LCD_PORT->ODR &= ~(LCD_DATA_BITS); // clear DATA bits
	LCD_PORT->ODR |= (command >> 4);   // DATA = command
	delay_us(50);
	LCD_pulse_ENA(); // latch HIGH NIBBLE
}

void LCD_command(uint8_t command)
{
	// send command to LCD in 4-bit instruction mode
	// HIGH nibble then LOW nibble, timing sensitive
	LCD_PORT->ODR &= ~(LCD_DATA_BITS);				   // isolate cmd bits
	LCD_PORT->ODR |= ((command >> 4) & LCD_DATA_BITS); // HIGH nibble
	delay_us(50);
	LCD_pulse_ENA(); // latch HIGH NIBBL

	LCD_PORT->ODR &= ~(LCD_DATA_BITS);			// isolate cmd bits
	LCD_PORT->ODR |= ((command)&LCD_DATA_BITS); // LOW shifted HIGH nibble
	delay_us(50);
	LCD_pulse_ENA(); // latch LOW NIBBLE
}

void LCD_write_char(uint8_t letter)
{
	// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
	LCD_PORT->ODR |= (LCD_RS); // RS = HI for data to address
	delay_us(500);
	LCD_command(letter);		// character to print
	LCD_PORT->ODR &= ~(LCD_RS); // RS = LO
}
