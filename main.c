#ifndef F_CPU
#define F_CPU 16000000UL // Frequencia de 16MHz
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "serial.h"
#include "HCSR.h"
#include "controle.h"

unsigned char valor = 0;
char buffer[100];
float referencia = 0;
char contador = 0;
char duty = 0;

float erro = 0;
float erro1 = 0;
float erro2 = 0;

int main(void)
{
    int num1, dec1 = 0;

    USART_Init(9600); //Inicia a serial
    HCSR_init(); //Inicia o HCSR
    USART_String("Sensores iniciados...\r\n");
    Controle_Init(); //Iniciar o valor de referência
    
    sprintf(buffer, "Valor %d \r\n", ((int) valor));  
    USART_String(buffer); 

    sei();

    while(1)                     
    {
        
        int centimetro = HCSR_centimetro(); //Lê centímetro
        
        //Realiza a conversão para litro
        float litro = HCSR_litro((float) centimetro);
        num1 = (litro*10)/10;
        dec1 = (litro*10) - (num1*10);
       

        sprintf(buffer, "Centimetro: %d | Litro: %d,%d \r\n", centimetro, ((int) num1), ((int) dec1) );
        USART_String(buffer);      

        Controle_leds(litro);
        Controle_duty(litro);
        Controle_PWM();
        

    }

}

ISR (TIMER0_COMPA_vect)  // Interrupcao TMR0
{
    
    contador++;

    if(contador < duty)
        PORTD |= (1<<PWM);
    else
        PORTD &= ~(1<<PWM);

    if(contador >= 100)
        contador = 0;
    
}