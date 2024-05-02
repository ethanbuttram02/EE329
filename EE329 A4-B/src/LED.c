#include "LED.h"

void Delay(uint32_t time)					//Delay Function
{
	for (int j = 0; j < time; j++) //For loop a do nothing command to create pseudo delay
			{
		; //Do nothing
	}
}

void LED_Clock(void)						//Binary 4 Bit Clock Function
{
	for (uint8_t i = 0; i <= 15; i++)		//Increment i by 1 from 0 to 15
			{
		GPIOC->ODR |= i;  //Turn on C Pins corresponding to value of i in binary
		Delay(50000);             		//Delay to create time between ticks
		GPIOC->BRR |= 0x000F;				//Turn off first four C port pins
	}
}

var_type TestFunction(var_type num) {//Function to test speed of function call

	var_type test_var;  						// local variable

	GPIOC->BSRR = (GPIO_PIN_1);             	// turn on PC1
	test_var = num % 10;					 	//Perform Test operation
	GPIOC->BRR = (GPIO_PIN_1);              	// turn off PC1

	return test_var;
}

void LED_Config(void) {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN); //Reset and Clock Control, Turns on Port C

	//Configure GPIO pins PC0, PC1 for:
	// output mode, push-pull, no pull up or pull down, high speed
	GPIOC->MODER &= ~(GPIO_MODER_MODE0 |		//Mode PC1..3 set to 00
			GPIO_MODER_MODE1 |
			GPIO_MODER_MODE2 |
			GPIO_MODER_MODE3);

	GPIOC->MODER |= (GPIO_MODER_MODE0_0 |	//Mode PC1..3 set to 01:output
			GPIO_MODER_MODE1_0 |
			GPIO_MODER_MODE2_0 |
			GPIO_MODER_MODE3_0);

	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 |
	GPIO_OTYPER_OT1 |
	GPIO_OTYPER_OT2 |
	GPIO_OTYPER_OT3);

	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 |
	GPIO_PUPDR_PUPD1 |
	GPIO_PUPDR_PUPD2 |
	GPIO_PUPDR_PUPD3);

	GPIOC->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos)
			| (3 << GPIO_OSPEEDR_OSPEED1_Pos) | (3 << GPIO_OSPEEDR_OSPEED2_Pos)
			| (3 << GPIO_OSPEEDR_OSPEED3_Pos));

	//Reset GPIO Pins 0-3
	GPIOC->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

