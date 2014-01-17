/*
 * Schieberegister.c
 *
 * Created: 24.12.2013 18:51:45
 *  Author: Christian
 */ 

#include <avr/io.h>
#include <stdlib.h>

#include "Schieberegister.h"

// Array fuer die Daten
unsigned char com74hc595[ COM74HC595_SIZE ];

void com74hc595_setBit( unsigned char BitNumber)
{
	unsigned char Port = BitNumber >> 3;
	unsigned char Bit = BitNumber & 7;
	com74hc595[ Port ] |= (1 << ( Bit ) );
}

void com74hc595_unsetBit( unsigned char BitNumber)
{
	unsigned char Port = BitNumber >> 3;
	unsigned char Bit = BitNumber & 7;
	com74hc595[ Port ] &= ~(1 << ( Bit ) );
}

void Com74hc595_setall(void)
{
	for ( int i = 0; i < COM74HC595_BYTES; i++ )
	{
		com74hc595_setBit( i );
	}
}

void com74hc595_unsetall(void)
{
	for ( int i = 0; i < COM74HC595_BYTES; i++ )
	{
		com74hc595_unsetBit( i );
	}
}

void com74hc595_setPort( unsigned char Port, unsigned char Bits )
{
	com74hc595[ Port ] = Bits;
}

void com74hc595_unsetPort( unsigned char Port )
{
	com74hc595[ Port ] = 0x00;
}

void com74hc595_init()
{
	/* Verwendete Ports auf OUT */
	DDR_COM74HC595 |= ( 1 << DDR_SER);
	DDR_COM74HC595 |= ( 1 << DDR_SCK);
	DDR_COM74HC595 |= ( 1 << DDR_RCK);
	
	#ifdef WITH_OE
	DDR_COM74HC595 |= ( 1 << DDR_OE);
	#endif
	
	/* SER auf definierten Level LOW */
	PORT_COM74HC595 &= ~(1<<PORT_SER);
	
	
	
	/* SCR und RCK auf definierten Level HIGH */
	PORT_COM74HC595 |= (1<<PORT_SCK);
	PORT_COM74HC595 |= (1<<PORT_RCK);
	
	#ifdef WITH_OE
	PORT_COM74HC595 |= (1<<PORT_OE);
	#endif
	
	com74hc595_unsetall();
	com74hc595_out();
	
	#ifdef WITH_OE
	PORT_COM74HC595 &= ~(1<<PORT_OE);
	#endif
}

void com74hc595_out()
{
	unsigned char anz = COM74HC595_SIZE;
	unsigned char* serp = com74hc595 + COM74HC595_SIZE;
	
	do
	{
		unsigned char bits;
		unsigned char data = *--serp;
		
		/* 8 Bits pro Byte rausschieben */
		for (bits = 8; bits > 0; bits--)
		{
			PORT_COM74HC595 &= ~(1<<PORT_SER);
			if (data & 0x80)
			{
				PORT_COM74HC595 |= (1<<PORT_SER);
			};
			
			data <<= 1;
			/* Strobe an SCK schiebt Daten im Gaensemarsch */
			/* um 1 Position weiter durch alle Schieberegister */
			PORT_COM74HC595 &= ~(1<<PORT_SCK);
			PORT_COM74HC595 |= (1<<PORT_SCK);
		}
	}
	while (--anz > 0);
	
	/* Strobe an RCK bringt die Daten von den Schieberegistern in die Latches */
	PORT_COM74HC595 &= ~(1<<PORT_RCK);
	PORT_COM74HC595 |= (1<<PORT_RCK);
}