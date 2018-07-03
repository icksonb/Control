#include <avr/io.h>
#include <avr/interrupt.h>
#include "HCSR.h"
#include <util/delay.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL // Frequencia de 16MHz
#endif

#define LED1 PIND2
#define LED2 PIND3
#define LED3 PIND4
#define PWM PIND6

extern unsigned char valor;

void Controle_init(void);
void Controle_leds(float litro);
void Controle_referencia(void);
void Controle_duty(float litro);
void Controle_IniciaPWM(void);
void habilita_TMR0(void);
void desabilita_TMR0(void);
void Controle_PWM(void);
void Controle_IniciaPWM(void);
char Controle_tendencia(void);
void Controle_ajustaDuty(char tendencia);
void Controle_FaixaLitro(float litro);