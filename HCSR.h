#include <avr/io.h>
#include <avr/interrupt.h>

#define TRIGGER PINB5
#define ECHO PINB4

#define RAIO 6
#define PI 3.14159
#define CM_MAX 19

void HCSR_init();
int HCSR_tempoResposta(void);
int HCSR_centimetro (void);
float HCSR_litro (float centimetro);
void delay_us(int tempo);
float mediana(float *vetor, int n);
int Transforma_centimetro (float centimetro);
void habilitaTMR1(void);
void desabilitaTMR1(void);