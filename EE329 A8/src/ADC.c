/*

Author: Ethan Buttram
Date: 5/30/24

*/

#include <ADC.h>
#include <main.h>
#include <uart.h>

uint16_t adcResult;
uint8_t dataReady = 0;

void ADC_init() {
    // initialize the ADC related GPIO and Registers

    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC

    // power up & calibrate ADC
    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
    ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
    ADC1->CR |= (ADC_CR_ADVREGEN);             // enable V regulator - see RM 18.4.6
    delay_us(20);                              // wait 20us for ADC to power up
    ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
    ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
    ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
    while (ADC1->CR & ADC_CR_ADCAL) {;}        // wait for calib to finish

    // enable ADC
    ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
    ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
    while(!(ADC1->ISR & ADC_ISR_ADRDY)) {;}    // wait for ADC Ready flag
    ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag

    // configure ADC sampling & sequencing
    ADC1->SQR1  |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
    ADC1->SMPR1 |= (7 << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time = 6.5 clocks
    ADC1->CFGR  &= ~( ADC_CFGR_CONT  |         // single conversion mode
                    ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
                    ADC_CFGR_RES   );        // 12-bit resolution

    // configure & enable ADC interrupt
    ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
    ADC1->ISR |= ADC_ISR_EOC;                  // set to clear EOC flag
    NVIC->ISER[0] = (1<<(ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
    __enable_irq();                            // enable global interrupts

    // configure GPIO pin PA0 
    RCC->AHB2ENR  |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
    GPIOA->MODER  |= (GPIO_MODER_MODE0);       // analog mode for PA0 (set MODER last)

    ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion 
}

void ADC1_2_IRQHandler(void) {
    // Check if the EOC (End of Conversion) interrupt flag is set
    if (ADC1->ISR & ADC_ISR_EOC) {
        // Clear the EOC flag by writing a 1 to it
        ADC1->ISR |= ADC_ISR_EOC;
        
        // Read the ADC conversion result
        adcResult = (uint16_t)ADC1->DR;
        dataReady = 1;
    }
    dataReady = 0;
}

// configure SysTick timer for use with delay_us().
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
void SysTick_Init(void)
{
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |    // enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk); // select CPU clock
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  // disable interrupt
}

// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : results in longer delays than specified:
//	   e.g. @4MHz, delay_us(1) = 10=15 us delay.
void delay_us(const uint32_t time_us)
{
    // set the counts for the specified delay
    SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
    SysTick->VAL = 0;                               // clear timer count
    SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk); // clear count flag
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
        ; // wait for flag
}