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

uint8_t numArray[4];
uint8_t voltArray[4];
uint16_t min = 0;
uint16_t max = 0;
uint16_t avg = 0;

void uart_init(void);
void LPUART_Print(const char *);
void LPUART1_IRQHandler(void);
void LPUART_ESC_Print(const char *escape);
void ADC_UART_init(void);
uint8_t *LPUART_Make_Counts(uint16_t num);
uint8_t *LPUART_Make_Volts(uint16_t num);
char *convertDigitsToChars(uint8_t* digits);
void printMin(uint16_t);
void printMax(uint16_t);
void printAvg(uint16_t);
#endif /* INC_UART_H_ */