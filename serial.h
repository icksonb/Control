#include <avr/io.h>


void USART_Init(unsigned int baud);
void USART_Transmit( unsigned char data );
void USART_String(char s[]);
unsigned char USART_Receive (void);