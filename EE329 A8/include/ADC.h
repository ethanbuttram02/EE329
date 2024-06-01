/*

Author: Ethan Buttram
Date: 5/30/24

*/

#include <main.h>

// function/variable signatures
uint16_t adcResult;
void ADC_init(void);
void ADC1_2_IRQHandler(void);
void SysTick_Init(void);
void delay_us(uint32_t);
