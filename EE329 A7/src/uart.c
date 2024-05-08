/*
 * uart.c
 *
 *  Created on: May 7, 2024
 *      Author: Ethan
 */
// boundaries vert: 3-29 hor: 11-99
#include "uart.h"
uint16_t hor = 55, vert = 16;
char center[] = "[16;55H";

void uart_init(void)
{
    PWR->CR2 |= (PWR_CR2_IOSV);                                                            // power avail on PG[15:2] (LPUART1)
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);                                                 // enable GPIOG clock
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;                                               // enable LPUART clock bridge
    GPIOG->MODER &= ~(GPIO_MODER_MODER7);                                                  // configure GP7 to AF
    GPIOG->MODER |= GPIO_MODER_MODER7_1;                                                   // Alternate function mode
    GPIOG->MODER &= ~(GPIO_MODER_MODER8);                                                  // Configure GP8 to AF
    GPIOG->MODER |= GPIO_MODER_MODER8_1;                                                   // Alternate function mode
    GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);                                // disable pu-pd
    GPIOG->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);                                 // set pp for GP7
    GPIOG->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos) | (3 << GPIO_OSPEEDR_OSPEED8_Pos)); // output speed high

    GPIOG->AFR[0] &= ~(0xF << GPIO_AFRL_AFSEL7_Pos); // For pin 7 (Tx)
    GPIOG->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL8_Pos); // For pin 8 (Rx)
    GPIOG->AFR[0] |= (8 << GPIO_AFRL_AFSEL7_Pos);    // For pin 7 (Tx)
    GPIOG->AFR[1] |= (8 << GPIO_AFRH_AFSEL8_Pos);    // For pin 8 (Rx)
    LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);  // 8-bit data
    LPUART1->CR1 |= USART_CR1_UE;                    // enable LPUART1
    LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);   // enable xmit & recv
    LPUART1->CR1 |= USART_CR1_RXNEIE;                // enable LPUART1 recv interrupt
    LPUART1->ISR &= ~(USART_ISR_RXNE);               // clear Recv-Not-Empty flag
    LPUART1->BRR = 8889;                             // set baud rate of 115200 for 4M
    NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));    // enable LPUART1 ISR
    __enable_irq();                                  // enable global interrupts
}

void LPUART_ESC_Print(const char *escape)
{
    while (!(LPUART1->ISR & USART_ISR_TXE))
        ;                    // wait for empty xmit buffer
    LPUART1->TDR = ESC_code; // prime with initial ESC code
    // while(!(LPUART1->ISR & USART_ISR_TC));  // wait for transmission
    while (*escape)
    {
        while (!(LPUART1->ISR & USART_ISR_TXE))
            ;
        LPUART1->TDR = *escape++;
    }
    while (!(LPUART1->ISR & USART_ISR_TC))
        ; // wait for transmission
}

void LPUART_Print(const char *message)
{
    uint16_t iStrIdx = 0;
    while (message[iStrIdx] != 0)
    {
        while (!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
            ;
        LPUART1->TDR = message[iStrIdx]; // send this character
        iStrIdx++;                       // advance index to next char
    }
}

void LPUART1_IRQHandler(void)
{
    uint8_t charRecv;
    if (LPUART1->ISR & USART_ISR_RXNE)
    {
        charRecv = LPUART1->RDR;
        switch (charRecv)
        {
        case 'R':
            LPUART_ESC_Print("[31m");
            break;
        case 'G':
            LPUART_ESC_Print("[32m");
            break;
        case 'B':
            LPUART_ESC_Print("[36m");
            break;
        case 'W':
            LPUART_ESC_Print("[37m");
            break;
        case 'w': // up
            // move up
            LPUART_ESC_Print("[2J");
            vert--;
            if (vert < 3)
            {
                center[1] = 2 + '0';
                center[2] = 9 + '0';
                vert = 29;
            }
            else
            {
                center[1] = (vert / 10) + '0';
                center[2] = (vert % 10) + '0';
            }
            LPUART_ESC_Print(center);
            LPUART_Print("o7");
            break;
        case 's': // down
            LPUART_ESC_Print("[2J");
            vert++;
            if (vert > 29)
            {
                center[1] = 0 + '0';
                center[2] = 3 + '0';
                vert = 3;
            }
            else
            {
                center[1] = (vert / 10) + '0';
                center[2] = (vert % 10) + '0';
            }
            LPUART_ESC_Print(center);
            LPUART_Print("o7");
            break;
        case 'd': // right
            LPUART_ESC_Print("[2J");
            hor++;
            if (hor > 99)
            {
                center[4] = 1 + '0';
                center[5] = 1 + '0';
                hor = 11;
            }
            else
            {
                center[4] = (hor / 10) + '0';
                center[5] = (hor % 10) + '0';
            }
            LPUART_ESC_Print(center);
            LPUART_Print("o7");
            break;
        case 'a': // left
            LPUART_ESC_Print("[2J");
            hor--;
            if (hor < 11)
            {
                center[4] = 9 + '0';
                center[5] = 9 + '0';
                hor = 99;
            }
            else
            {
                center[4] = (hor / 10) + '0';
                center[5] = (hor % 10) + '0';
            }
            LPUART_ESC_Print(center);
            LPUART_Print("o7");
            break;
        default:
            while (!(LPUART1->ISR & USART_ISR_TXE))
                ;                    // wait for empty TX buffer
            LPUART1->TDR = charRecv; // echo char to terminal
        }                            // end switch
    }
}
