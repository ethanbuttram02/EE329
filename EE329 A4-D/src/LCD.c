/*
 * LCD.c
 *
 *  Created on: Apr 22, 2024
 *      Author: eugie
 */
#include "LCD.h"

void LCD_Config(void) {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIODEN); //Reset and Clock Control, Turns on Port C

	//Configure GPIO pins PC0, PC1 for:
	// output mode, push-pull, no pull up or pull down, high speed
	GPIOD->MODER  &=  ~(GPIO_MODER_MODE0 |
						GPIO_MODER_MODE1 |
						GPIO_MODER_MODE2 |
						GPIO_MODER_MODE3 |
						GPIO_MODER_MODE4 |
						GPIO_MODER_MODE5);

	GPIOD->MODER  |=   (GPIO_MODER_MODE0_0 |
						GPIO_MODER_MODE1_0 |
						GPIO_MODER_MODE2_0 |
						GPIO_MODER_MODE3_0 |
						GPIO_MODER_MODE4_0 |
						GPIO_MODER_MODE5_0);

	GPIOD->OTYPER &=  ~(GPIO_OTYPER_OT0 |
						GPIO_OTYPER_OT1 |
						GPIO_OTYPER_OT2 |
						GPIO_OTYPER_OT3 |
						GPIO_OTYPER_OT4 |
						GPIO_OTYPER_OT5);

	GPIOD->PUPDR  &=  ~(GPIO_PUPDR_PUPD0 |
						GPIO_PUPDR_PUPD1 |
						GPIO_PUPDR_PUPD2 |
						GPIO_PUPDR_PUPD3 |
						GPIO_PUPDR_PUPD4 |
						GPIO_PUPDR_PUPD5);

	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos)|
						(3 << GPIO_OSPEEDR_OSPEED1_Pos)|
						(3 << GPIO_OSPEEDR_OSPEED2_Pos)|
						(3 << GPIO_OSPEEDR_OSPEED3_Pos)|
						(3 << GPIO_OSPEEDR_OSPEED4_Pos)|
						(3 << GPIO_OSPEEDR_OSPEED5_Pos));

	//Reset GPIO Pins 0-5
	GPIOD->BRR    =    (GPIO_PIN_0 |
						GPIO_PIN_1 |
						GPIO_PIN_2 |
						GPIO_PIN_3 |
						GPIO_PIN_4 |
						GPIO_PIN_5);
}

void LCD_init(void) {
	delay_us(40000);                     // power-up wait 40 ms
	for (int idx = 0; idx < 3; idx++) {  // wake up 1,2,3: DATA = 0011 XXXX
		LCD_4b_command(0x30);             // HI 4b of 8b cmd, low nibble = X
		delay_us(200);
	}

	LCD_4b_command(0x20); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
	delay_us(40);         // remainder of LCD init removed - see LCD datasheets
	LCD_command(0x28);
	LCD_command(0x10); //Set cursor
	LCD_command(0x0F); //Display ON; Cursor ON
	LCD_command(0x06); //Entry mode set
	LCD_command(0x01); //Clear
}

void LCD_pulse_ENA(void) {
	// ENAble line sends command on falling edge
	// set to restore default then clear to trigger
	LCD_PORT->ODR |= ( LCD_EN);         	// ENABLE = HI
	delay_us(500);                         	// TDDR > 320 ns
	LCD_PORT->ODR &= ~( LCD_EN);          	// ENABLE = LOW
	delay_us(500);                         	// low values flakey, see A3:p.1
}

void LCD_4b_command(uint8_t command) {
	// LCD command using high nibble only - used for 'wake-up' 0x30 commands
	LCD_PORT->ODR &= ~( LCD_DATA_BITS); 	// clear DATA bits
	LCD_PORT->ODR |= (command >> 4);     	// DATA = command
	delay_us(500);
	LCD_pulse_ENA();
}

void LCD_command(uint8_t command) {
	// send command to LCD in 4-bit instruction mode
	// HIGH nibble then LOW nibble, timing sensitive
	LCD_PORT->ODR &= ~( LCD_DATA_BITS);                 // isolate cmd bits
	LCD_PORT->ODR |= ((command >> 4) & LCD_DATA_BITS);  // HIGH shifted low
	delay_us(500);
	LCD_pulse_ENA();                                    // latch HIGH NIBBLE

	LCD_PORT->ODR &= ~( LCD_DATA_BITS);               	// isolate cmd bits
	LCD_PORT->ODR |= (command & LCD_DATA_BITS);      	// LOW nibble
	delay_us(500);
	LCD_pulse_ENA();                                    // latch LOW NIBBLE
}

void LCD_write_char(uint8_t letter) {
	// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
	LCD_PORT->ODR |= (LCD_RS);       	// RS = HI for data to address
	delay_us(500);
	LCD_command(letter);             	// character to print
	LCD_PORT->ODR &= ~(LCD_RS);      	// RS = LO
}

void LCD_write_string(char *word) {  	//Takes in pointer for string address
	while (*word) {						//While data at address is not equal to NULL
		LCD_write_char(*word++);		//Write data letter and increment to next letter
	}
}

void LCD_setup_counter(void) {
	LCD_command(0x01);					  //Clear
	LCD_write_string("EE 329 A3 TIMER");  //Set first line of display
	LCD_command(0xC0);					  //Set Cursor to 2nd line
	LCD_write_string("*=SET #=GO 00:00"); //Set second line of display
	LCD_command(0xCF);					  //Sets cursor to end of the line
}

void LCD_count_start(char min, char sec) {
	delay_us(1000000);
	while (min >= 0) {					  //Goes until no minutes left
		while (sec > 0) {				  //Goes until no seconds left
			GPIOC->ODR |= 1;			  //Blink LED before display switches
			delay_us(95000);
			GPIOC->BRR = 1;
			sec--;						  //Decrements second
			LCD_command(0xCE);			  //Sets cursor to seconds place
			LCD_write_char((sec / 10) + 48); //Writes ascii seconds count
			LCD_write_char((sec % 10) + 48);
			for (int i = 0; i < 9; i++) { //Checks for star interrupt
				delay_us(74000);		  //and delays for one second
				if (Keypad_star_press()) {
					return;
				}
			}
		}

		if (min == 0) {					  //Breaks if minutes equals zero
			break;
		}
		min--;							  //Decrements minutes
		LCD_command(0xCB);				  //Sets cursor to minutes place
		LCD_write_char((min / 10) + 48);	  //Writes ascii minutes count
		LCD_write_char((min % 10) + 48);
		sec = 60;						  //Sets seconds to 60
	}
	LED_Clock();						  //Plays LED "Dance" when finished
}

