/*
 * analyzer_functions.c
 *
 * 
 *  Author: PiotrPC
 */
#define F_CPU 16000000UL
#include "analyzer_functions.h"
#include <util/delay.h>
#include "../lcdpcf8574/lcdpcf8574.h"
#include "../pcf8574/pcf8574.h"
#include <stdlib.h>

void heating(int HEAT_TIME)
{
	lcd_clrscr();
	lcd_puts("Heating sensor");
	lcd_gotoxy(0, 1);
	lcd_puts("Seconds left:");
	
	
	PORTB |= 1 << PORTB1;	//œwiecenie diody podczas rozgrzewania
	
	uint16_t i;
	char sek[3];
	for (i = HEAT_TIME; i > 0; i--)
	{	
		lcd_gotoxy(14, 1);
		itoa(i, sek, 10);
		lcd_puts(sek);
		_delay_ms(1000);
	}

	lcd_clrscr();
	PORTB &= ~(1 << PORTB1);	//zgaszenie diody po rozgrzaniu
}

void alarm(int number_of_beeps)
{
	for (int i = 0; i < number_of_beeps; i++)
	{
		//PORTB |= 1 << PORTB0;	//buzzer high level trigger
		PORTB &= ~(1<<PORTB0);		//buzzer low level trigger
		PORTB |= 1 << PORTB1;	//dioda (ni¿ej kolejnoœæ taka sama)
		_delay_ms(50);
		//PORTB &= ~(1 << PORTB0);
		PORTB |= 1<<PORTB0;
		PORTB &= ~(1 << PORTB1);
		_delay_ms(400);
	}
}

void start_message()
{
	lcd_clrscr();
	lcd_puts("  Breathalyser");
	lcd_gotoxy(0, 1);
	lcd_puts("________________");
	_delay_ms(2000);
	lcd_clrscr();
	lcd_puts("Made by");
	lcd_gotoxy(0, 1);
	lcd_puts("Piotr Wojsa");
	_delay_ms(2000);
	lcd_clrscr();
	lcd_puts("and");
	lcd_gotoxy(0, 1);
	lcd_puts("Maciej Kaminski");
	_delay_ms(2000);

}

void options()
{
	lcd_clrscr();
	lcd_puts("Press 1 to start");
	lcd_gotoxy(0, 1);
	lcd_puts("the measurement");
	_delay_ms(2000);
	lcd_clrscr();
	lcd_puts("Press 2 to check");
	lcd_gotoxy(0, 1);
	lcd_puts("latest results");
	_delay_ms(2000);
	lcd_clrscr();
	lcd_puts("Press 3 to");
	lcd_gotoxy(0, 1);
	lcd_puts("change the unit ");
	_delay_ms(2000);
	lcd_clrscr();
	lcd_puts("Please select an");
	lcd_gotoxy(0, 1);
	lcd_puts("option you want");
}