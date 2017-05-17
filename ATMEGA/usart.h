#ifndef _USART_H_
#define _USART_H_

#include <util/delay.h>
#include <avr/io.h>

//USART functions
extern int	uart_putc (unsigned char c);
extern void uart_puts (char *s);
extern void USART_Init (uint8_t ubrr);
extern void USART_SendByte (uint8_t u8Data);
extern void uart_Send_Int_Array (uint8_t input [], uint8_t len);

#endif /* _USART_H_ */