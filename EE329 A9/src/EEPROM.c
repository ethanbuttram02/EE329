/* Author: Ethan Buttram */
/* Date: 5/22/2024 */

#include <EEPROM.h>
#include <stm32l496xx.h>

void EEPROM_init() {
    /* USER configure GPIO pins for I2C alternate functions SCL and SDA */
    // Configure I2C 
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;  // enable I2C bus clock
    I2C1->CR1   &= ~( I2C_CR1_PE );        // put I2C into reset (release SDA, SCL)
    I2C1->CR1   &= ~( I2C_CR1_ANFOFF );    // filters: enable analog
    I2C1->CR1   &= ~( I2C_CR1_DNF );       // filters: disable digital
    I2C1->TIMINGR = 0x00303D5B;            // 16 MHz SYSCLK timing from CubeMX
    I2C1->CR2   |=  ( I2C_CR2_AUTOEND );   // auto send STOP after transmission
    I2C1->CR2   &= ~( I2C_CR2_ADD10 );     // 7-bit address mode
    I2C1->CR1   |=  ( I2C_CR1_PE );        // enable I2C

    // build EEPROM transaction

    I2C1->CR2   &= ~( I2C_CR2_RD_WRN );    // set WRITE mode
    I2C1->CR2   &= ~( I2C_CR2_NBYTES );    // clear Byte count
    I2C1->CR2   |=  ( 3 << I2C_CR2_NBYTES_Pos); // write 3 bytes (2 addr, 1 data)
    I2C1->CR2   &= ~( I2C_CR2_SADD );      // clear device address
    I2C1->CR2   |=  ( 0x51 << (I2C_CR2_SADD_Pos+1) ); // device addr SHL 1
    I2C1->CR2   |=    I2C_CR2_START;       // start I2C WRITE op

    /* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */

    while(!(I2C1->ISR & I2C_ISR_TXIS)) ;   // wait for start condition to transmit
    I2C1->TXDR = (0x51 >> 8); // xmit MSByte of address
    /* address high, address low, data  -  wait at least 5 ms before READ 
    the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */

}

void EEPROM_read() {
    
}

void EEPROM_write(char* message) {

}