/*
 * keypad.c
 *
 *  Created on: Apr 16, 2024
 *      Author: Seth Saxena
 */

#include "keypad.h"
#include "stm32l4xx_hal.h"
#include <math.h>

void Keypad_Config(void)
{
	// output mode, push-pull, no pull up or pull down, high speed
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIODEN); // enables GPIO clock
	// COLUMNS
	// clear bits in pins PD4-PD7
	COL_PORT->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
						 GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	// sets pins PD4-PD7 to output mode
	COL_PORT->MODER |= (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
						GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);
	// sets pins PD4-PD7 to push pull
	COL_PORT->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |
						  GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
	// disables PU and PD resistors for pin PD4-PD7
	COL_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |
						 GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
	// sets speed for PD4-PD7 to high
	COL_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED4_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED6_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED7_Pos));
	// initializes PD4-PD7 to 1 (HI output)
	COL_PORT->BSRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

	// ROWS
	// sets PD0-PD3 to Input Mode
	ROW_PORT->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
						 GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	// sets pins PD0-PD3 to push pull
	ROW_PORT->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 |
						  GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
	// clears bits in PUPDR corresponding to pin PD0-PD3
	ROW_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
						 GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	// activates pull down resistors for PD4-PD7
	ROW_PORT->PUPDR |= (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1 |
						GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);
	// sets speed for PD4-PD7 to high
	ROW_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED3_Pos));
}

// -----------------------------------------------------------------------------
int Keypad_IsAnyKeyPressed(void)
{
	// drive all COLUMNS HI; see if any ROWS are HI
	// return true if a key is pressed, false if not
	// set all columns HI
	COL_PORT->BSRR = COL_PINS;
	// DEBOUNCE
	int key = 0;
	/* BEGIN: Code from Michael Mazzola */
	while (key == 0)
	{
		while ((ROW_PORT->IDR & ROW_PINS) == 0)
		{
			; // Wait for key press
		}
		for (int i = 0; i < SETTLE; i++)
		{
			; // Delay
		}
		if ((ROW_PORT->IDR & ROW_PINS) != 0)
			key = 1;
	}
	/* END: Code from Michael Mazzola */
	if ((ROW_PORT->IDR & ROW_PINS) != 0)
	{ // got a keypress!
		return (TRUE);
	}
	else
		return (FALSE); // nope.
}

// -----------------------------------------------------------------------------
int Keypad_WhichKeyIsPressed(void)
{
	// detect and encode a pressed key at {row,col}
	// assumes a previous call to Keypad_IsAnyKeyPressed() returned TRUE
	// verifies the Keypad_IsAnyKeyPressed() result (no debounce here),
	// determines which key is pressed and returns the encoded key ID

	int8_t iRow = 0, iCol = 0, iKey = 0; // keypad row & col index, key ID result
	int8_t bGotKey = 0;					 // bool for keypress, 0 = no press

	COL_PORT->BSRR = COL_PINS; // set all columns HI
	for (iRow = 0; iRow < NUM_ROWS; iRow++)
	{ // check all ROWS
		if (ROW_PORT->IDR & (BIT0 << iRow))
		{								// keypress in iRow!!
			COL_PORT->BRR = (COL_PINS); // set all cols LO
			for (iCol = 0; iCol < NUM_COLS; iCol++)
			{										   // 1 col at a time
				COL_PORT->BSRR = (BIT0 << (4 + iCol)); // set this col HI
				if (ROW_PORT->IDR & (BIT0 << iRow))
				{ // keypress in iCol!!
					bGotKey = 1;
					break; // exit for iCol loop
				}
			}
			if (bGotKey)
				break;
		}
	}
	//	Row 1: '1'=0 '2'=1 '3'=2 'A'=3, ROW 2: '4'=4 '5'=5 '6'=6 'B'=7
	//	Row 3: '7'=8 '8'=9 '9'=10 'C'=11 ROW 4: '*'=12 '0'=13 '#'=14 'D'=15
	//   no press: send NO_KEYPRESS
	if (bGotKey)
	{
		iKey = (iRow * NUM_COLS) + iCol; // handles all Key Presses
		// if ( iKey == KEY_ZERO )   //commented out because we use 4x4 keypad
		//      iKey = CODE_ZERO;
		return (iKey); // return encoded keypress
	}
	return (NO_KEYPRESS); // unable to verify keypress
}
