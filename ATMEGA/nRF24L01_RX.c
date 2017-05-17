/*
 * nRF24L01_RX.c
 *
 * Created: 04.04.2016
 * Author: Maximilian Voigt
 */ 
#ifndef F_CPU				//define F_CPU if not done 
#define F_CPU 16000000UL	//16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "usart.h"
#include "spi.h"
#include "wl_module.h"
#include "nRF24L01.h"
#include "animation.h"

#define BAUD				57600UL							//38400UL
#define UBRR				((F_CPU+BAUD*8)/(BAUD*16)-1)	

uint8_t payload[wl_module_PAYLOAD];							//holds the payload
uint8_t timercounter = 0, standby = 0;

	
int main(void)
{
	char ready [] = "nRF24L01 RX is ready!\r\n";
	DDRC = 0xFF;
	
	USART_Init (UBRR);		//initialise usart
	wl_module_init();		//Init nRF24L01 Module
	animation_init();
	
	//TCCR0B |= ( (1<<CS02) | (1<<CS00)); //Timer aktivieren
	//TIMSK0 |= ( (1<<TOIE0));
	
	PORTC = (1 << PORTC5);
	_delay_ms(100);			//wait for Module
	PORTC = (0 << PORTC5);
	sei();

	wl_module_config();		//config nRF24L01 as RX Module, simple Version
	uart_puts (ready);
	
	_delay_ms(100);			//wait to start
	
    while(1)
    {	
		PORTC = (0 << PORTC5);
		while (standby | !wl_module_data_ready());			//waits for RX_DR Flag in STATUS

		wl_module_get_data(payload);						//reads the incomming Data to Array payload
		PORTC = (1 << PORTC5);
		animation_step(payload);
    }
}

ISR(TIMER0_OVF_vect)
{
	timercounter++;
	if(timercounter >= 60)
	{
		timercounter = 0;
		standby = 1;
	}
}

/*
ISR(INT0_vect)
{
    uint8_t status;   
    
        // Read wl_module status 
        wl_module_CSN_lo;                               // Pull down chip select
        status = spi_fast_shift(NOP);					// Read status register
        wl_module_CSN_hi;                               // Pull up chip select
		PORTC = (1 << PORTC5);
		
		if (status & (1<<RX_DR))							// IRQ: incomming
		{
			wl_module_CE_lo;
			wl_module_get_data(payload);
			wl_module_CE_hi;
		}
		
		//PORTC = (0 << PORTC5);
}
*/