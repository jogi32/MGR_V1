/*
 * MGR_V1.c
 *
 * Created: 2015-05-03 16:52:41
 *  Author: Tomasz
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "HD44780.h"

//definicja ADCIN (wejœcie ADC)
//#define ADCINBOT PC0
//#define ADCINBOT PC1
#define ADCINBOT PC2
#define ADCINTOP PC3
#define ADC0			(0<<MUX2) |(0<<MUX1) | (0<<MUX0)
#define ADC1			(0<<MUX2) |(0<<MUX1) | (1<<MUX0)
#define ADC2			(0<<MUX2) |(1<<MUX1) | (0<<MUX0)
#define ADC3			(0<<MUX2) |(1<<MUX1) | (1<<MUX0)

#define MOTORRIGHTON	PORTB |= _BV(PB5)
#define MOTORRIGHTOFF	PORTB &= ~_BV(PB5)
#define MOTORLEFTTON	PORTB |= _BV(PB4)
#define MOTORLEFTTOFF	PORTB &= ~_BV(PB4)

#define MOTORUPON		PORTB |= _BV(PB3)
#define MOTORUPOFF		PORTB &= ~_BV(PB3)
#define MOTORDOWNON		PORTB |= _BV(PB2)
#define MOTORDOWNOFF	PORTB &= ~_BV(PB2)

#define GLUE(x, y) x##y
 
char bufor[16];

uint8_t W[4];
uint16_t W1;
uint16_t W2;
uint16_t W3;
uint16_t W4;
int16_t DifrX;
int16_t DifrY;
int16_t Difr;


void ADC_init();
void ADC_mes();
void Pos_init();
uint16_t MaxFromArray(uint8_t* array);

int main(void)
{
	DDRB |= _BV(PB5);
	
	
	
	ADC_init();
	LCD_Initalize();
	ADCSRA |= (1<<ADSC);//Bit 6 – ADSC: ADC Start Conversion (uruchomienie pojedynczej konwersji
	while(ADCSRA & (1<<ADSC));//czeka na zakoñczenie konwersji
	itoa(ADC,bufor,10);
	LCD_WriteText(bufor);
    while(1)
    {
		Pos_init();
		LCD_Clear();
		//ADCSRA |= (1<<ADSC);//Bit 6 – ADSC: ADC Start Conversion (uruchomienie pojedynczej konwersji
		//while(ADCSRA & (1<<ADSC));//czeka na zakoñczenie konwersji
		//itoa(ADC, bufor,10);
		//LCD_WriteText(bufor);
		itoa(Difr,bufor,10);
		LCD_WriteText("DIFF: ");	LCD_WriteText(bufor);
		_delay_ms(100);
    }
}

void ADC_init()
{
	//Uruchomienie ADC, wewnêtrzne napiecie odniesienia, tryb pojedynczej konwersji, preskaler 128, wejœcie PIN5, wynik do prawej
	ADCSRA |= 	 (1<<ADEN)//Bit 7 – ADEN: ADC Enable (uruchomienie przetwornika)
	|(1<<ADPS0)
	|(1<<ADPS1)
	|(1<<ADPS2);//ADPS2:0: ADC Prescaler Select Bits (ustawienie preskalera) preskaler= 128

	ADMUX  =	 (1<<REFS1) | (1<<REFS0)//Bit 7:6 – REFS1:0: Reference Selection Bits
	//Internal 1.1V Voltage Reference with external capacitor at AREF pin
	|(1<<MUX1) | (1<<MUX0);//Input Channel Selections (ADC5 - Pin 5 )
	
	DDRC &=~ (1<<PC0);            //Ustawienie Wejœcia ADC
	DDRC &=~ (1<<PC1);            //Ustawienie Wejœcia ADC
	DDRC &=~ (1<<ADCINBOT);            //Ustawienie Wejœcia ADC
	DDRC &=~ (1<<ADCINTOP);            //Ustawienie Wejœcia ADC
	
}

void ADC_mes()
{
	ADCSRA |= (1<<ADSC);//Bit 6 – ADSC: ADC Start Conversion (uruchomienie pojedynczej konwersji
	while(ADCSRA & (1<<ADSC));//czeka na zakoñczenie konwersji
}
void Pos_init()
{
	ADMUX = (1<<REFS1) | (1<<REFS0) | GLUE(ADC, 0);
	ADC_mes();
	W1 = ADC;
	W[0] = W1 / 4;
	ADMUX = (1<<REFS1) | (1<<REFS0) | GLUE(ADC, 1);
	ADC_mes();
	W2 = ADC;
	W[1] = W2 / 4;
	ADMUX = (1<<REFS1) | (1<<REFS0) | GLUE(ADC, 2);
	ADC_mes();
	W3 = ADC;
	W[2] = W3 / 4;
	ADMUX = (1<<REFS1) | (1<<REFS0) | GLUE(ADC, 3);
	ADC_mes();
	W4 = ADC;
	W[3] = W4 / 4;
	Difr = MaxFromArray(W);
	DifrX = W1 - W2;
	DifrY = W3 - W4;
	
	
	if (fabs(DifrX) > Difr)
	{
		if (	DifrX > 0	)
		{
			MOTORRIGHTOFF;
			MOTORLEFTTON;
		} 
		else if (	DifrX < 0	)
		{
			MOTORLEFTTOFF;
			MOTORRIGHTON;
		}
	} 
	else
	{
		MOTORRIGHTOFF;
		MOTORLEFTTOFF;
	}
	
	
	
	if (fabs(DifrY) > Difr)
	{
		if (	DifrY > 0	)
		{
			MOTORUPOFF;
			MOTORDOWNON;
		}
		else if (	DifrY < 0	)
		{
			MOTORDOWNOFF;
			MOTORUPON;
		}
	}
	else
	{
		MOTORDOWNOFF;
		MOTORUPOFF;
	}
}

uint16_t MaxFromArray(uint8_t* array)
{
	uint8_t max = array[0];
	for(int i = 1; i<4; i++)
	{
		if(array[i] > max)
			max = array[i];
	}
	return max;
}