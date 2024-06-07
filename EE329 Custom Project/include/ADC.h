/*

Author: Ethan Buttram
Date: 5/30/24

*/

#include <main.h>

// function/variable signatures
extern uint16_t adcResult;
extern const uint8_t SAMPLE_SIZE;
extern uint16_t samples[];
extern uint8_t idx;
extern uint8_t dataReady;
void ADC_init(void);
void ADC1_2_IRQHandler(void);
void SysTick_Init(void);
void delay_us(uint32_t);
