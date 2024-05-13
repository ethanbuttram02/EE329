/* 
******************************************************

Author: Ethan Buttram 
Date: 13 May, 2024
Name: DAC.h

******************************************************
*/

#include "stm32l4xx_hal.h"

void DAC_init(void);
void DAC_convert(uint16_t val);
void DAC_write(uint16_t val);