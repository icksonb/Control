#include <avr/io.h>
#include "serial.h"

#ifndef F_CPU
#define F_CPU 16000000UL // Frequencia de 16MHz
#endif

void USART_Init(unsigned int baud)
{
	unsigned int ubrr = 0;
	ubrr = F_CPU/16/baud-1;
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data )
{
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = data;
}

unsigned char USART_Receive (void)
{
    while(!(UCSR0A) & (1<<RXC0));                  
    return UDR0;                                   
}

void USART_String(char s[])
{
	int i =0;
	
	while (s[i] != 0x00)
	{
		USART_Transmit(s[i]);
		i++;
	}
}