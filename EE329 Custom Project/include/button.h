// Author: Ethan buttram
// Date: 6/7/2024

#ifndef BUTTON_H
#define BUTTON_H

#include "stm32l4xx.h"

#define BUTTON_PIN_1 GPIO_PIN_0 // PB0: CN10 - 21
#define BUTTON_PIN_2 GPIO_PIN_1 // PB1: CN10 - 7
#define BUTTON_PIN_3 GPIO_PIN_2 // PB2: CN9 - 13
#define BUTTON_PIN_4 GPIO_PIN_3 // PB3: CN7 - 15
#define BUTTON_PIN_5 GPIO_PIN_4 // PB4: CN7 - 11
#define BUTTON_PORT GPIOB       // use port B pins

// Function to initialize buttons and set up interrupts
void button_init(void);

// Custom button interrupt handlers
void BTN0_IRQHandler(void);
void BTN1_IRQHandler(void);
void BTN2_IRQHandler(void);
void BTN3_IRQHandler(void);
void BTN4_IRQHandler(void);

#endif // BUTTON_H
