#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "HCSR.h"
#include <util/delay.h>
#include "controle.h"

extern unsigned char valor;
extern char buffer[100];
extern float referencia;
extern char duty;
extern char contador;
extern float erro;
extern float erro1;
extern float erro2;

float minimoDuty = 0;
float maximoDuty = 0;
char faixa = 0;
char faixaAntiga = 0;

void habilita_TMR0(void)
{
	TCCR0B |= (1<<CS02) | (1<<CS00); //Prescaler de 1024
}

void desabilita_TMR0(void)
{
	TCCR0B &= ~(1<<CS02) & ~(1<<CS00); 
}

void Controle_Init(void)
{
	DDRD |= (1 << LED1) | (1 << LED2) | (1 << LED3); //Inicia os leds de controle
	PORTD |= (1 << LED1) | (1 << LED2) | (1 << LED3); //Apaga todos os leds

	valor = 0;
	
	USART_String("Informe o valor de controle:\r\n");
    //Ler o valor de controle a ser utilizado
    USART_String("1 - 0,5 Litro\r\n");
    USART_String("2 - 0,7 Litro\r\n");
    USART_String("3 - 1 Litros\r\n");
    USART_String("4 - 1,2 Litros\r\n");
    USART_String("5 - 1,4 Litros\r\n");
    

    while (valor == 0)
    {
        valor = USART_Receive();
    }

    Controle_referencia();
    Controle_IniciaPWM();

}

void Controle_leds(float litro)
{
	if(litro < (float) 0.5)
	{
		PORTD |= (1 << LED1);
		PORTD |= (1 << LED2);
		PORTD |= (1 << LED3);
	}

	if(litro > (float) 0.5)
	{
		PORTD &= ~(1 << LED1);
		PORTD |= (1 << LED2);
		PORTD |= (1 << LED3);
	}

	if(litro >= (float)  1.0)
	{
		PORTD &= ~(1 << LED1);
		PORTD &= ~(1 << LED2);
		PORTD |= (1 << LED3);
	}

	if(litro >= (float) 1.2)
	{
		PORTD &= ~(1 << LED1);
		PORTD &= ~(1 << LED2);
		PORTD &= ~(1 << LED3);
	}

} 

void Controle_referencia(void)
{
	switch(valor)
	{
		case 1:
			referencia = 0.5;
			break;

		case 2:
			referencia = 0.7;
			break;

		case 3:
			referencia = 1.0;
			break;
		
		case 4:
			referencia = 1.2;
			break;

		case 5:
			referencia = 1.4;
			break;

		default:
			referencia = 0;
			break;
	}
}

void Controle_duty(float litro)
{
	desabilita_TMR0();
	
	erro2 = erro1;
	erro1 = erro;
	erro = referencia - litro;

	if(erro > 0)
	{
		//Verifica tendencia
		char tendencia = 0;
		tendencia = Controle_tendencia();

		//Verifica faixa de ativação
		Controle_FaixaLitro(litro);

		//Ajusta o duty
		Controle_ajustaDuty(tendencia);	
	}
	else
	{
		duty = 0;
	}
	

	habilita_TMR0();
}

void Controle_FaixaLitro(float litro)
{
	float porcento = 0;
	porcento = (litro*100) / referencia;

	faixaAntiga = faixa;

	if(porcento <= 60)
	{
		minimoDuty = 70;
		maximoDuty = 100;
		faixa = 1;
	}
	else
	{
		if(porcento < 80)
		{
			minimoDuty = 60;
			maximoDuty = 80;
			faixa = 2;
		}
		else
		{
			minimoDuty = 50;
			maximoDuty = 70;	
			faixa = 3;
		}
	}
}

void Controle_ajustaDuty(char tendencia)
{
	if(tendencia != 0)
	{
		//Se sair da faixa, recebe o valor mínimo
		if(faixa != faixaAntiga)
		{
			duty = minimoDuty;
		}
		//Se continuar na faixa
		else
		{
			switch(tendencia)
			{
				case 1:
					duty = duty + 5;
					break;
				case 2:
					duty = duty;
					break;
				case 3:
					duty = duty + 10;
					break;
			}
		}
	}

	if(duty > maximoDuty)
		duty = maximoDuty;

	if(duty < minimoDuty)
		duty = minimoDuty;
}


/*********************************************************
* CALCULA TENDENCIA DA CURVA DE ERRO (CHEGAR AO SET POINT)
* 0 - PERTURBAÇÃO
* 1 - TENDENCIA DE DIMINUIR ERRO 
* 2 - TENDENCIA DE MANTER O ERRO
* 3 - TENDENCIA DO ERRO AUMENTAR
*********************************************************/
char Controle_tendencia(void)
{
	if(erro2 > erro1 && erro1 > erro)
		return 1;

	if(erro2 == erro1 && erro1 == erro2)
		return 2;

	if(erro2 < erro1 && erro1 < erro)
		return 3;

	if( (erro2 > erro1 && erro1 < erro) || (erro2 < erro1 && erro1 > erro))
		return 0;
}

void Controle_IniciaPWM(void)
{
	//TCCR0A |= (1<<WGM01); //Modo CTC
	OCR0A = 156; //Para 10ms
	DDRD |= (1<<PWM); //Saída
	PORTD &= (1<<PWM); //Desabilita
	habilita_TMR0();
}

void Controle_PWM(void)
{
	int aux = 0;

    for (aux=0; aux<16; aux++) //Para 100ms
    {
        do
        {
            if(contador < duty)
                PORTD |= (1<<PWM);
            else
                PORTD &= ~(1<<PWM);

        }while(TCNT0 <= 100);

        TCNT0 = 0;    
    }
}
