/*
 *Projekt_alkomat.c
 *
 *Created: 07.06.2020 16:12:13
 *Author : PiotrPC
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "lcdpcf8574/lcdpcf8574.h"
#include "pcf8574/pcf8574.h"
#include "debounce/debounce.h"
#include "analyzer_functions/analyzer_functions.h"

int R2 = 4700;
int R0 = 140;
int number_of_beeps = 0;
int HEAT_TIME = 5;
double sensor_volt;
double RS_gas;
double ratio;
double BAC;
double x;
double RESULT = 0.0;
double PREVIOUS = 0.0;
char bufor[16];
char unit[4]="ppm";



int main(void)
{
	DDRB |= 1<<DDB0;
	DDRB |= 1<<DDB1;
	//DDRB = 0b111111;  //klawiatura
	PORTB |= 1<<PORTB0;
	PORTB &= ~(1 << PORTB1);
	//PORTB = 0b111000;  //klawiatura

	TCCR0B |= 1 << CS02;
	TIMSK0 |= 1 << TOIE0;

	debounce_init();

	lcd_init(LCD_DISP_ON);
	lcd_home();

	/*LCD test */
	lcd_led(0);

	start_message();
	heating(HEAT_TIME);

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (0 << ADLAR);
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= 1 << ADEN;

	options();
	
	sei();
	
	while (1)
	{
		if (button_down(BUTTON1_MASK))
		{
			char sek;
			PREVIOUS = RESULT;
			lcd_clrscr();
			lcd_puts("Please blow into");
			lcd_gotoxy(0,1);
			lcd_puts("sensor for ");
			for (int i = 5; i >= 1; i--)
			{
				sek = i + '0';
				lcd_putc(sek);
				lcd_puts(" sec");
				PORTB |= 1 << PORTB1;
				lcd_gotoxy(11, 1);
				_delay_ms(1000);
				ADCSRA |= 1 << ADSC;
				RESULT = RESULT + BAC;
			}
			RESULT = RESULT / 5;
			PORTB &= ~(1 << PORTB1);
			dtostrf(RESULT, 2, 2, bufor);
			lcd_clrscr();
			lcd_puts("Concentration of");
			lcd_gotoxy(0, 1);
			lcd_puts("alcohol:");
			lcd_gotoxy(8, 1);
			lcd_puts(bufor);
			lcd_puts(unit);
			if ( (RESULT > 0.2 && !strcmp(unit,"ppm")) || (RESULT > 0.1 && !strcmp(unit,"mg/L")) )
			{
				alarm(5);
			}
			else
			{
				alarm(1);
			}
			_delay_ms(6000);
			options();



		}
		else if (button_down(BUTTON2_MASK))
		{
			lcd_clrscr();
			lcd_puts("Previous results:");
			dtostrf(RESULT, 2, 2, bufor);
			lcd_gotoxy(0, 1);
			lcd_puts("1.  ");
			lcd_puts(bufor);
			lcd_puts(unit);
			_delay_ms(3000);
			lcd_clrscr();
			lcd_puts("Previous results:");
			dtostrf(PREVIOUS, 2, 2, bufor);
			lcd_gotoxy(0, 1);
			lcd_puts("2.  ");
			lcd_puts(bufor);
			lcd_puts(unit);
			_delay_ms(3000);
			options();


		}
		else if (button_down(BUTTON3_MASK)) {
			if (!strcmp(unit,"ppm"))
			{	
				strcpy(unit,"mg/L");
				PREVIOUS = PREVIOUS / 2.1;
				RESULT = RESULT / 2.1;
				lcd_clrscr();
				lcd_puts("Unit changed");
				lcd_gotoxy(0,1);
				lcd_puts("It`s mg/L now");
			}
			else 
			{	
				strcpy(unit,"ppm");
				PREVIOUS = PREVIOUS * 2.1;
				RESULT = RESULT * 2.1;
				lcd_clrscr();
				lcd_puts("Unit changed");
				lcd_gotoxy(0,1);
				lcd_puts("It`s ppm now");
			}
			_delay_ms(3000);
			options();

		}

		
	}
}

ISR(ADC_vect)
{
	cli();
	
	double wynik = ADC;
	sensor_volt = wynik / 1024 * 5.0;
	RS_gas = ((5.0 *R2) / sensor_volt) - R2;
	ratio = RS_gas / R0;
	x = 0.4 * ratio;
	BAC = pow(x, -1.431);
	if (strcmp(unit,"ppm "))
	{
		BAC = BAC * 2.1;
	}
	
	
	sei();
	
}

ISR(TIMER0_OVF_vect)
{
	debounce();
}