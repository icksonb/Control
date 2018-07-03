#include <avr/io.h>
#include <avr/interrupt.h>
#include "HCSR.h"

void desabilitaTMR1(void)
{
	TCCR1B &= ~(1 << CS11);
}

void habilitaTMR1(void)
{
	TCNT1 = 0;
	TCCR1B |= (1 << CS11); //Prescaler de 8
}

void delay_us(int tempo)
{
	TCNT1 = 0;	
	habilitaTMR1();
	while (TCNT1 < (tempo*2)) {}
	desabilitaTMR1();
}

void HCSR_init()
{
	DDRB |= (1 << TRIGGER); //Saída
	DDRB &= ~(1 << ECHO); //Entrada
	
	desabilitaTMR1();

	TCCR1A = 0;                //confira timer para operação normal
	TCCR1B = 0;                //limpa registrador

	//TCNT1 = 0;
	//TCCR1B |= (1 << CS11); //Prescaler de 8
	
}

int HCSR_tempoResposta(void)
{
	
	PORTB &= ~(1 << TRIGGER); //Nível baixo
	delay_us(2);
	PORTB |= (1 << TRIGGER); //Nível alto
	delay_us(12);
	PORTB &= ~(1 << TRIGGER); //Nível baixo
	
	while ( ((PINB & (1 << ECHO)) >> ECHO) == 0 )  { } //Espera o nível ficar em alto
	
	TCNT1 = 0;	
	habilitaTMR1();
	while ( ((PINB & (1 << ECHO)) >> ECHO) == 1 )  { } //Espera ficar em nível baixo
	int retorno = 0;
	retorno = TCNT1/2;
	desabilitaTMR1();
	
	return ( (int) retorno);
}

float mediana(float *vetor, int n)
{
	int i,j = 0;
	float a = 0;
	for (i = 0; i < n; ++i) 
    {
        for (j = i + 1; j < n; ++j)
        {
            if (vetor[i] > vetor[j]) 
            {
                a =  vetor[i];
                vetor[i] = vetor[j];
                vetor[j] = a;
            }
        }
    }

    return vetor[2];
}


int HCSR_centimetro (void)
{
	float centimetro[5] = {0,0,0,0,0};
	int tempo = 0;
	int i = 0;

	for (i=0; i<5; i++)
	{
		tempo = HCSR_tempoResposta();
		centimetro[i] = (float) ( (((float)tempo) *  ((float) 0.0172)) ); // 1/58 = 0.0172	
	}
	
	if( Transforma_centimetro(mediana(centimetro, 5)) < CM_MAX)
		return Transforma_centimetro(mediana(centimetro, 5));
	else
		return ( (int) CM_MAX);


}

float HCSR_litro (float centimetro)
{
	float litro = 0;
	float cm = ( (float) CM_MAX) - ( (float) centimetro);
	litro = ((float) PI) * ( (float) (((float) RAIO)/10) * (((float) RAIO)/10)) * ( (float) ( (float) cm / 10));

	return litro; //0.1 de correção
}

int Transforma_centimetro (float centimetro)
{
	int num1, num2, dec1, numero_cm = 0;
        
    num1 = (centimetro * 100)/1000;
    num2 = (centimetro*100 - (num1*1000))/100;
    dec1 = (centimetro*100 - (num1*1000 + num2*100))/10;
    
    if(dec1 > 7)
        numero_cm = num1*10 + num2 + 1;
    else
        numero_cm = num1*10 + num2;

    return numero_cm;
}