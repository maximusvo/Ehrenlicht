/*
 * usart.c
 *
 * Created: 12.06.2016 17:57:38
 * Author: Maximilian Voigt
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usart.h"

int uart_putc (unsigned char c)
{
	while (!(UCSR0A&(1<<UDRE0)));
	
	UDR0 = c;
	return 0;
}

void uart_puts (char *s)
{
	while (*s)
	{
		uart_putc(*s);
		s++;
	}
}

void uart_Send_Int_Array (uint8_t input[], uint8_t len)
{
	char stringBuffer [4*len];
	char end [] = "\r\n";
	char space [] = "  ";	
	uint8_t i;
	
	for (i = 0; i < len; ++i)
	{
		snprintf(stringBuffer, 4*len, "%d", input[i]);
		uart_puts(stringBuffer);
		uart_puts(space);
	}
	uart_puts(end);
}


void USART_Init (uint8_t ubrr)
{
        UBRR0H = (uint8_t)(8<<ubrr);
        UBRR0L = (uint8_t) ubrr;
        //Enable receiver and transmitter
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
        //Set frame format: 8data, 1stop bit
        UCSR0C = (3<<UCSZ00);	
}

void USART_SendByte (uint8_t u8Data)
{
    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0);
    // Transmit data
    UDR0 = u8Data;  
}