#include "keypad.h"

void Keypad_Config(void) {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); //Reset and Clock Control, Turns on Port D

	GPIOA->MODER &= ~(GPIO_MODER_MODE0 |		//Mode PD0..3 set to 00
			GPIO_MODER_MODE1 |
			GPIO_MODER_MODE2 |
			GPIO_MODER_MODE3);

	GPIOA->MODER |= (GPIO_MODER_MODE0_0 |	//Mode PC0..3 set to 01:output
			GPIO_MODER_MODE1_0 |
			GPIO_MODER_MODE2_0 |
			GPIO_MODER_MODE3_0);

	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT0 |		//OTYPE Set to 0:
			GPIO_OTYPER_OT1 |
			GPIO_OTYPER_OT2 |
			GPIO_OTYPER_OT3);

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0 |		//PUPD Set to 00:pushpull
			GPIO_PUPDR_PUPD1 |
			GPIO_PUPDR_PUPD2 |
			GPIO_PUPDR_PUPD3);

	GPIOA->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos)
			|	//Output speed set high
			(3 << GPIO_OSPEEDR_OSPEED1_Pos) | (3 << GPIO_OSPEEDR_OSPEED2_Pos)
			| (3 << GPIO_OSPEEDR_OSPEED3_Pos));

//-------------------------------------------------------------------------------------------

	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOFEN); //Reset and Clock Control, Turns on Port F

	GPIOF->MODER &= ~(GPIO_MODER_MODE0 |	//Mode PC1..3 set to 00
			GPIO_MODER_MODE1 |
			GPIO_MODER_MODE2 |
			GPIO_MODER_MODE3);

	GPIOF->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk |
	GPIO_PUPDR_PUPD1_Msk |
	GPIO_PUPDR_PUPD2_Msk |
	GPIO_PUPDR_PUPD3_Msk);

	GPIOF->PUPDR |= (GPIO_PUPDR_PUPD0_1 |		//PUPDR set to 1b'10: Input
			GPIO_PUPDR_PUPD1_1 |
			GPIO_PUPDR_PUPD2_1 |
			GPIO_PUPDR_PUPD3_1);

	GPIOA->BRR = (GPIO_PIN_0 |					//Resets GPIOD Pins 0-3
			GPIO_PIN_1 |
			GPIO_PIN_2 |
			GPIO_PIN_3);

	GPIOF->BRR = (GPIO_PIN_0 |					//Resets GPIOF Pins 0-3
			GPIO_PIN_1 |
			GPIO_PIN_2 |
			GPIO_PIN_3);
}

// -----------------------------------------------------------------------------

uint16_t Keypad_IsAnyKeyPressed(void) {
// drive all COLUMNS HI; see if any ROWS are HI
// return true if a key is pressed, false if not
	COL_PORT->BSRR |= COL_PINS;        	 				// set all columns HI

	for (uint16_t idx = 0; idx < 10000; idx++) {			//Button debounce
		;
	}
	if ((ROW_PORT->IDR &= 0x0F) != 0)       				//got a keypress!
			{
		return (1);
	} else {
		return (0);
	}                          				//nope.
}

// -----------------------------------------------------------------------------
uint16_t Keypad_WhichKeyIsPressed(void) {
// detect and encode a pressed key at {row,col}
// assumes a previous call to Keypad_IsAnyKeyPressed() returned TRUE
// verifies the Keypad_IsAnyKeyPressed() result,
// determines which key is pressed and returns the encoded key ID
	int8_t iRow = 0, iCol = 0, iKey = 0; // keypad row & col index, key ID result
	int8_t bGotKey = 0;             		// bool for keypress, 0 = no press

	COL_PORT->BSRR = COL_PINS;                       	 // set all columns HI
	for (iRow = 0; iRow < 4; iRow++) {      	 			// check all ROWS
		if ( ROW_PORT->IDR & (GPIO_PIN_0 << iRow)) {      // keypress in iRow!!
			COL_PORT->BRR = ( COL_PINS);            	 	// set all cols LO
			for (iCol = 0; iCol < 3; iCol++) {   			// 1 col at a time
				COL_PORT->BSRR = ( GPIO_PIN_0 << (iCol));     // set this col HI
				if ( ROW_PORT->IDR & (GPIO_PIN_0 << iRow)) { // keypress in iCol!!
					bGotKey = 1;
					break;                                 // exit for iCol loop
				}
			}
			if (bGotKey)
				break;
		}
	}
	//	encode {iRow,iCol} into LED word : row 1-3 : numeric, ‘1’-’9’
	//	                                   row 4   : ‘*’=10, ‘0’=15, ‘#’=12
	//                                    no press: send NO_KEYPRESS
	if (bGotKey) {
		iKey = (iRow * 3) + iCol + 1;  		// handle numeric keys ...
		if (iKey == KEY_ZERO)               // If input is zero key,
			iKey = 0;							//sets return variable to zero
		return (iKey);                       // return encoded keypress
	}
	return ( NO_KEYPRESS);                   // unable to verify keypress
}

uint8_t Keypad_star_press(void) {

	uint16_t press = 0;						//initializing press and key vars
	uint16_t key = 0;
	press = Keypad_IsAnyKeyPressed();			//Checks for key press
	if (press) {
		key = Keypad_WhichKeyIsPressed();		//Checks which key is pressed
		Keypad_debounce(key);
		if (key == 10)			//Does nothing if 13 is returned from function
				{
			return 1;
		}
	}
	return 0;
}
void Keypad_debounce(uint8_t key) {
	while (key == Keypad_WhichKeyIsPressed()) {
		;										//Waits until key is not pressed
	}
}

