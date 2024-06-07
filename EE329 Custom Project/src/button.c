// Author: Ethan buttram
// Date: 6/7/2024

#include <stm32l4xx.h>  // Adjust the include according to your MCU series
#include <main.h>
#include <button.h>

void button_init(void) {
    // Enable the clock for GPIO port B
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    
    // Configure pins as input
    BUTTON_PORT->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);

    // Enable pull-down resistors (if necessary)
    BUTTON_PORT->PUPDR |= (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1 | GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);

    // Configure EXTI lines for the buttons
    SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PB | SYSCFG_EXTICR1_EXTI1_PB | SYSCFG_EXTICR1_EXTI2_PB | SYSCFG_EXTICR1_EXTI3_PB;
    
    // Enable rising edge trigger for the EXTI lines
    EXTI->RTSR1 |= (EXTI_RTSR1_RT0 | EXTI_RTSR1_RT1 | EXTI_RTSR1_RT2 | EXTI_RTSR1_RT3);
    
    // Enable interrupts for the EXTI lines
    EXTI->IMR1 |= (EXTI_IMR1_IM0 | EXTI_IMR1_IM1 | EXTI_IMR1_IM2 | EXTI_IMR1_IM3);
    
    // Enable the interrupt in the NVIC
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF0) {
        EXTI->PR1 = EXTI_PR1_PIF0;  // Clear the pending bit
        BTN0_IRQHandler();  // Call custom handler
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF1) {
        EXTI->PR1 = EXTI_PR1_PIF1;  // Clear the pending bit
        BTN1_IRQHandler();  // Call custom handler
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF2) {
        EXTI->PR1 = EXTI_PR1_PIF2;  // Clear the pending bit
        BTN2_IRQHandler();  // Call custom handler
    }
}

void EXTI3_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF3) {
        EXTI->PR1 = EXTI_PR1_PIF3;  // Clear the pending bit
        BTN3_IRQHandler();  // Call custom handler
    }
}

void EXTI4_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF4) {
        EXTI->PR1 = EXTI_PR1_PIF4;  // Clear the pending bit
        BTN4_IRQHandler();  // Call custom handler
    }
}

void BTN0_IRQHandler(void) {
    // Button 0 will change units
    switch(unitFlag) {
        case 0: unitFlag = 1; break;
        case 1: unitFlag = 0; break;
    }
}

void BTN1_IRQHandler(void) {
    // Button 1 will be power on/off
}

void BTN2_IRQHandler(void) {
    // Button 2 will
}

void BTN3_IRQHandler(void) {
    // Button 3 will
}

void BTN4_IRQHandler(void) {
    // Handle button 4 press
}
