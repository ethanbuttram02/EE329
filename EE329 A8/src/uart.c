/*
 * uart.c
 *
 *  Created on: May 7, 2024
 *      Author: Ethan
 */

#include <uart.h>
#include <main.h>
#include <ADC.h>

uint8_t numArray[4];
uint8_t voltArray[4];
uint16_t min = 0;
uint16_t max = 0;
uint16_t avg = 0;
uint16_t voltage = 0;

// defining cursor locations for the beginnings of values on the terminal
#define minCountPos "[2;6H"
#define minVoltPos "[2;12H"
#define maxCountPos "[3;6H"
#define maxVoltPos "[3;12H"
#define avgCountPos "[4;6H"
#define avgVoltPos "[4;12H"
#define coilCurrentPos "[5;16H"


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
        case 'r':
            // r, this is how we initialize everything. r for "run" or "reset"
            LPUART_ESC_Print("[0;1H");  // resets cursor
            LPUART_ESC_Print("[2J");
            ADC_UART_init();            // sets up menu
        break;

        case 's':
            /* 
            s for start, begins data collection, at any time we can reset the
            table and halt data collection for different calibration tests 
            without reuploading code
             */
            //while(charRecv != 'r') {
                charRecv = LPUART1->RDR;
                uint16_t samples[20] = {2200, 1000, 3500, 1600, 300, 2800, 3700, 2950, 1400, 200, 900, 700, 1800, 1500, 3660, 1900, 2400, 2600, 2800, 3900};
                int sum = 0;
                
                for (int i = 0; i < 20; i++) {
                    
                    sum += samples[i];

                    if (min == 0 || (samples[i] < min)) {
                        min = samples[i];
                    }

                    if (max == 0 || (samples[i] > max)) {
                        max = samples[i];
                    }
                }

                avg = sum / 20;

                printMin(min);
                printMax(max);
                printAvg(avg);
                printCoilCurrent(9);
            //}

            // reinit values to 0
            min = 0;
            max = 0;
            avg = 0;
        break;
            
        default:
            while (!(LPUART1->ISR & USART_ISR_TXE));    // wait for empty TX buffer
            LPUART1->TDR = charRecv;
            break;                    // echo char to terminal
        }                                               // end switch
    }
}

void ADC_UART_init() {
    /* this function will display the default menu:
        ADC counts volts
        MIN  0000  0.000 V
        MAX  0000  0.000 V
        AVG  0000  0.000 V
        to modify the counts and volts value, I will use LPUART_ESC_Print to move the cursor to the line and overwrite the value there
    */

   LPUART_ESC_Print("[1;0H");            // sets cursor to origin (0,0)
   LPUART_Print("ADC counts volts");     // line 1
   LPUART_ESC_Print("[2;0H");            // newline
   LPUART_Print("MIN  0000  0.000 V");   // line 2
   LPUART_ESC_Print("[3;0H");            // newline
   LPUART_Print("MAX  0000  0.000 V");   // line 3
   LPUART_ESC_Print("[4;0H");            // newline
   LPUART_Print("AVG  0000  0.000 V");   // line 4
   LPUART_ESC_Print("[5     ;0H");            // newline
   LPUART_Print("coil current = 0.000 A");  // coil current line
   LPUART_ESC_Print("[0;0H");            // resets cursor
}

uint8_t* LPUART_Make_Counts(uint16_t num) {
   numArray[0] = num / 1000;  //take thousands place
   numArray[1] = (num / 100) % 10;  //take hundreds place
   numArray[2] = (num / 10) % 10;  //take tens place
   numArray[3] = num % 10;  //take ones place
   return numArray;
}

uint8_t* LPUART_Make_Volts(uint16_t num){
    voltage = (num * 3300)/4095; //convert number to voltage
    voltArray[0] = voltage / 1000; //grab thousands place (V)
    voltArray[1] = (voltage / 100) % 10; //grab hundreds place (100mV)
    voltArray[2] = (voltage / 10) % 10; //grab tens place (10mV)
    voltArray[3] = voltage % 10; //grab ones place (1mV)
    return voltArray;
}


char* convertDigitsToChars(uint8_t* digits) {
    // Allocate memory for the resulting char array (5 characters: 4 digits + 1 null terminator)
    char* charArray = (char*)malloc(5 * sizeof(char));
    if (charArray == NULL) return NULL; // Check for allocation failure

    // Convert each digit to the corresponding character
    for (uint8_t i = 0; i < 4; i++) {
        charArray[i] = digits[i] + '0'; // Convert digit to character, adding '0' converts to ASCII 
    }
    // Null-terminate the string
    charArray[4] = '\0';

    return charArray;
}

void printMin(uint16_t min) {
    // Print minCountPos
    LPUART_ESC_Print(minCountPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Counts(min)));

    LPUART_ESC_Print(minVoltPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Volts(min)));

    LPUART_ESC_Print(minVoltPos);
    LPUART_ESC_Print("[2;13H");
    LPUART_Print(convertDigitsToChars(LPUART_Make_Volts(min)));

    LPUART_ESC_Print("[2;13H");
    LPUART_Print(".");
}

void printMax(uint16_t max) {
    // Print minCountPos
    LPUART_ESC_Print(maxCountPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Counts(max)));

    LPUART_ESC_Print(maxVoltPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Volts(max)));

    LPUART_ESC_Print(maxVoltPos);
    LPUART_ESC_Print("[3;13H");
    LPUART_Print(convertDigitsToChars(LPUART_Make_Volts(max)));

    LPUART_ESC_Print("[3;13H");
    LPUART_Print(".");
}

void printAvg(uint16_t avg) {
    // Print minCountPos
    LPUART_ESC_Print(avgCountPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Counts(avg)));

    LPUART_ESC_Print(avgVoltPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Volts(avg)));

    LPUART_ESC_Print(avgVoltPos);
    LPUART_ESC_Print("[4;13H");
    LPUART_Print(convertDigitsToChars(LPUART_Make_Volts(avg)));

    LPUART_ESC_Print("[4;13H");
    LPUART_Print(".");
}

void printCoilCurrent(uint16_t val) {
    uint16_t current = (val * 1000) / 8;
    LPUART_ESC_Print(coilCurrentPos);
    LPUART_Print(convertDigitsToChars(LPUART_Make_Counts(current))); 

    LPUART_ESC_Print(coilCurrentPos);
    LPUART_ESC_Print("[5;17H");
    LPUART_Print(convertDigitsToChars(LPUART_Make_Counts(current)));

    LPUART_ESC_Print("[5;17H");
    LPUART_Print(".");
}