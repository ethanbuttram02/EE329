/* 
******************************************************

Author: Ethan Buttram 
Date: 13 May, 2024
Name: DAC.c

******************************************************
*/

#include "stm32l4xx_hal.h"
#include "DAC.h"

void DAC_init(void) {
    // initialize the SPI peripheral to communicate with the DAC

}

uint8_t *DAC_convert(uint16_t val) {
    // convert a voltage to a 12 bit word to control the DAC
    // 4096 discrete voltages from Vss to Vdd (0 to 3.3V)
    // goal: write to an array the binary representation and iterate over it with SPI.
    uint8_t dataArr[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (val == 0) {
        // logic to drive
    }
    uint16_t word = val * (3.3 / 4096);
}

void DAC_write(uint16_t val) {
    // write a 16-bit word including control and data bits to the DAC

}