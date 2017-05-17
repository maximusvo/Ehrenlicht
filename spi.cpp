/*
master spi
*/

#include "spi.h"
#include <avr/io.h>
#include <avr/interrupt.h>
 
/* 
PORT_SPI    SPIC -> PORTC
DD_MISO     PIN6
DD_MOSI     PIN5
DD_SS       PIN4 -> in wl_module.c defined
DD_SCK      PIN7
*/
 
void spi_init()
// Initialize pins for spi communication
{
    PORTC.DIRSET &= ~(PIN7_bm|PIN6_bm|PIN5_bm);
    // Define the following pins as output: MOSI, SS, SCK
    PORTC.DIRSET |= (PIN7_bm|PIN5_bm);
 
	SPIC.CTRL = 0xD0;
			/*	((1<<CLK2X)	|	 // SPI Clock Rate (1 = Double Clock Rate)
				 (1<<ENABLE)|	 // SPI Enable 
				 (0<<DORD)	|	 // Data Order (0:MSB first / 1:LSB first)
				 (1<<MASTER)|	 // Master/Slave select  
				 (0<<MODE)	|	 // 0 = Leading edge (Rising, sample), Trailing edge (Falling, setup)
				 (0<<PRESCALER));// SPI Clock Rate CLK/2
			 */
	SPIC.INTCTRL = 0x00;		 // SPI Interrupts off
	
}
 
void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPIC.DATA = dataout[i];
             while(!(SPIC.STATUS & SPI_IF_bm));
             datain[i] = SPIC.DATA;
       }
}
 
void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPIC.DATA = dataout[i];
             while(!(SPIC.STATUS & SPI_IF_bm));
       }
}
 
uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
    SPIC.DATA = data;
    while(!(SPIC.STATUS & SPI_IF_bm));
    return SPIC.DATA;
}