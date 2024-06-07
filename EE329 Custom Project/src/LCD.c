/* USER CODE BEGIN Header */
   /****************************************************************************
   * EE 329 A3 Parallel LCD
   *****************************************************************************
   * @file           : lcd.c
   * @brief          : Configure keypad, key press detection, key identification
   * project         : EE 329 - Spring 2024 - A2
   * authors         : Michael Mazzola - mfmazzol@calpoly.edu
   *                 : Katelynn Hoang - khoang19@calpoly.edu
   * 				 : Ethan Buttram - ebuttram@calpoly.edu
   * version         : 0.1
   * date            : 04-21-2024
   * compiler        : GNU Tools for STM32 (12.3rel1)
   * target          : NUCLEO-L4A6ZG
   * clocks          : 4MHz MSI to AHB2
   * @Description : This program provides support functions to be used to access
   *                the lcd interface. It allows initializing the interface and
   *                then supports writing to and reading from the display.
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

#include "lcd.h"
#include "delay.h"


/* -----------------------------------------------------------------------------
 * LCD_init:
 * Operation : Configure LCD pins. Set GPIOA pins 1-6 to output, no PU/PD, very
 *             high speed. It then runs the powerup sequence to put the display
 *             in 4 bit mode.
 * Inputs    : None
 * Outputs   : None
 * Locals    : None
 * TODO      : None
 * BUGS      : None
 * Citation  : EE329 Lab Manual. (2024-Apr.-17). https://docs.google.com/
 *             document/d/1xA1AfZJOFKy3r1e9WllB8o4h1oyOlbHH/
 *             edit#heading=h.j8sehv
 * -------------------------------------------------------------------------- */
void LCD_init( void )  {
   // Enable Port C
   RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOCEN);

   // Clear bits of interest
   GPIOC->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
                       GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
                       GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
                       GPIO_MODER_MODE6);

   // Set ports 0-6 to be output
   GPIOC->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 |
                       GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
                       GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
                       GPIO_MODER_MODE6_0 );

   // Clear bits of interest, sets output to be push-pull
   GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 |
                       GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
                       GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |
                       GPIO_OTYPER_OT6 );

   // Clear bits of interest, sets output to have no PU/PD resistor
   GPIOC->PUPDR  &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
                      GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
                      GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |
                      GPIO_PUPDR_PUPD6);

   // Sets outputs to be Very High Speed (~80 MHz, Datasheet | Valid??)
   GPIOC->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED3_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED4_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED6_Pos));

   // Preset outputs to zero
   GPIOC->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                 GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                 GPIO_PIN_6);

   delay_us( 40000 );                     // power-up wait 40 ms
   for ( int idx = 0; idx < 3; idx++ ) {  // wake up 1,2,3: DATA = 0011 XXXX
      LCD_4b_command( 0x30 );             // HI 4b of 8b cmd, low nibble = X
      delay_us( 200 );
   }
   LCD_4b_command( 0x20 ); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
   delay_us( 40 );
   LCD_command(0x28);      // fcn set 4b 2-line mode
   LCD_command(0x10);      // cursor on
   LCD_command(0x0F);      // Display ON with blinking cursor
   LCD_command(0x06);      // Entry Mode set (I=1, increment by 1), 0x00000110
}


/* -----------------------------------------------------------------------------
 * LCD_pulse_ENA:
 * Operation : Pulse enable. Controlled on falling edge of enable.
 * Inputs    : None
 * Outputs   : None
 * Locals    : None
 * TODO      : None
 * BUGS      : None
 * Citation  : EE329 Lab Manual. (2024-Apr.-17). https://docs.google.com/
 *             document/d/1xA1AfZJOFKy3r1e9WllB8o4h1oyOlbHH/
 *             edit#heading=h.j8sehv
 * -------------------------------------------------------------------------- */
void LCD_pulse_ENA( void )  {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
   LCD_PORT->ODR   |= ( LCD_EN );         // ENABLE = HI
   delay_us( 5 );                         // TDDR > 320 ns
   LCD_PORT->ODR   &= ~( LCD_EN );        // ENABLE = LOW
   delay_us( 5 );                         // low values flakey, see A3:p.1
}


/* -----------------------------------------------------------------------------
 * LCD_4b_command:
 * Operation : Uses high nibble for wake up commands of display to put into 4
 *             bit mode.
 * Inputs    : None
 * Outputs   : None
 * Locals    : None
 * TODO      : None
 * BUGS      : None
 * Citation  : EE329 Lab Manual. (2024-Apr.-17). https://docs.google.com/
 *             document/d/1xA1AfZJOFKy3r1e9WllB8o4h1oyOlbHH/
 *             edit#heading=h.j8sehv
 * -------------------------------------------------------------------------- */
void LCD_4b_command( uint8_t command )  {
// LCD command using high nibble only - used for 'wake-up' 0x30 commands
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );    // clear DATA bits
   LCD_PORT->ODR   |= ( command >> 4 );   // DATA = command
   delay_us( 5 );
   LCD_pulse_ENA( );
}


/* -----------------------------------------------------------------------------
 * LCD_command:
 * Operation : Used for running the LCD in 4 bit mode. Takes the 8 bit
 *             instruction and shifts into two 4 bit segments. It then sends
 *             them one after the other to the display.
 * Inputs    : None
 * Outputs   : None
 * Locals    : None
 * TODO      : None
 * BUGS      : None
 * Citation  : EE329 Lab Manual. (2024-Apr.-17). https://docs.google.com/
 *             document/d/1xA1AfZJOFKy3r1e9WllB8o4h1oyOlbHH/
 *             edit#heading=h.j8sehv
 * -------------------------------------------------------------------------- */
void LCD_command( uint8_t command )  {
// send command to LCD in 4-bit instruction mode
// HIGH nibble then LOW nibble, timing sensitive
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( (command>>4) & LCD_DATA_BITS ); // HIGH shifted low
   delay_us( 5 );
   LCD_pulse_ENA( );                                    // latch HIGH NIBBLE

   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( command & LCD_DATA_BITS );      // LOW nibble
   delay_us( 5 );
   LCD_pulse_ENA( );                                    // latch LOW NIBBLE
}


/* -----------------------------------------------------------------------------
 * LCD_write_char:
 * Operation : Writes one character to the display at a time. Takes in the letter
 *             at a time in its ASCII form and writes that to the LCD using the
 *             LCD command function.
 * Inputs    : None
 * Outputs   : None
 * Locals    : None
 * TODO      : None
 * BUGS      : None
 * Citation  : EE329 Lab Manual. (2024-Apr.-17). https://docs.google.com/
 *             document/d/1xA1AfZJOFKy3r1e9WllB8o4h1oyOlbHH/
 *             edit#heading=h.j8sehv
 * -------------------------------------------------------------------------- */
void LCD_write_char( uint8_t letter )  {
// calls LCD_command() w/char data; assumes all ctrl bits set LOW in LCD_init()
   LCD_PORT->ODR   |= (LCD_RS);       // RS = HI for data to address
   delay_us( 5 );
   LCD_command( letter );             // character to print
   LCD_PORT->ODR   &= ~(LCD_RS);      // RS = LO
}


/* -----------------------------------------------------------------------------
 * LCD_write_string:
 * Operation : This function calls the write char function succesively for each
 *             letter in the string. By doing so, it is able to write a string
 *             to the display one letter at a time.
 * Inputs    : None
 * Outputs   : None
 * Locals    : None
 * TODO      : None
 * BUGS      : None
 * Citation  : Patrick Crandall, EE 329 S'24 student. (2024-Apr.-24).
 *             Personal communication: Patrick demonstrated how he had
 *             implemented the write string function using write char.
 *
 * -------------------------------------------------------------------------- */
void LCD_write_string( const char str[] )  {
   for ( int i=0; str[i] != '\0'; i++)  {
      delay_us( 1400 );
      LCD_write_char( str[i] );
   }
}


/* -----------------------------------------------------------------------------
 * LCD_update_display:
 * Operation : Takes in two values for the minutes and seconds. Uses sprintf
 *             to combine these into a local buffer and then write these
 *             to the LCD using the LCD_write_string function.
 * Inputs    : None
 * Outputs   : None
 * Locals    : char Buffer[5]
 * TODO      : None
 * BUGS      : None
 * Citation  : EE329 Lab Manual. (2024-Apr.-17). https://docs.google.com/
 *             document/d/1xA1AfZJOFKy3r1e9WllB8o4h1oyOlbHH/
 *             edit#heading=h.j8sehv
 * -------------------------------------------------------------------------- */
void LCD_update_display(int minutes, int seconds) {
   LCD_command(LCD_COUNT_ADR);
   char buffer[5];
   sprintf(buffer, "%02d:%02d", minutes, seconds);
   LCD_write_string(buffer);
}

/* -----------------------------------------------------------------------------
 * LCD_update_display:
 * Operation : sets cursor to position detailed by the datasheet
 * Inputs    : uint8_t row, uint8_t col
 * Outputs   : None
 * Locals    : uint8_t address
 * TODO      : None
 * BUGS      : None
 * Citation  : None
 * -------------------------------------------------------------------------- */
void LCD_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? col : (col + 0x40);
    LCD_command(LCD_SET_DDRAM_ADDR | address);
}

