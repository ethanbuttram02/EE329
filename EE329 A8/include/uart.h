/*
 * uart.h
 *
 *  Created on: May 7, 2024
 *      Author: Ethan
 */
#ifndef INC_UART_H_
#define INC_UART_H_
#define ESC_code (0x1B)
#include "stm32l4xx_hal.h"
void uart_init(void);
void LPUART_Print(const char *);
void LPUART1_IRQHandler(void);
void LPUART_ESC_Print(const char *escape);
void ADC_UART_init(void);
void LPUART_Make_Counts(uint16_t num);
void LPUART_Make_Volts(uint16_t num);
void convertDigitsToChars(uint8_t* digits);
void printMin(void);
void printMax(void);
void printAvg(void);
#endif /* INC_UART_H_ */