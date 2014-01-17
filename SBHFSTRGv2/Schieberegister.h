/*
 * Schieberegister.h
 *
 * Created: 24.12.2013 18:52:20
 *  Author: Christian
 */


#ifndef SCHIEBEREGISTER_H_
#define SCHIEBEREGISTER_H_

#define SER_PIN PB3
#define SCHIEBE_PORT PORTB
#define SCHIEBE_DDR DDRB
#define RCK_PIN PB2
#define SCK_PIN PB5

// Anzahl der über Software-SPI anzusteuernde 74HC595 Bausteine
#define COM74HC595_SIZE 1

// Anzahl der Bits über alle Bausteine
#define COM74HC595_BYTES COM74HC595_SIZE * 8

// PORT am Mikrokontroller über den der Software SPI erfolgen soll
#define PORT_COM74HC595 SCHIEBE_PORT
#define PORT_SER 		SER_PIN		// 74HC595 PIN 14
#define PORT_SCK 		SCK_PIN		// 74HC595 PIN 11
#define PORT_RCK 		RCK_PIN		// 74HC595 PIN 12
//#define PORT_OE 		PB3		// 74HC595 PIN 13

#define DDR_COM74HC595  SCHIEBE_DDR
#define DDR_SER 		PORT_SER	        // 74HC595 PIN 14
#define DDR_SCK 		PORT_SCK	        // 74HC595 PIN 11
#define DDR_RCK 		PORT_RCK	        // 74HC595 PIN 12
//#define DDR_OE 			DDB3	        // 74HC595 PIN 13

/*
Falls eine Steuerung über /OE PIN 13 des 74HC595 gewünscht ist,
muss PIN 13 per Pullup Widerstand gesetzt werden, dass sorgt
dafür, dass der 74HC595 nach einem Reset in einem definierten
Zustand gesetzt werden kann.
Der 74HC595 wird in der Initialisierungsroutine mit 0x00 initialisiert.
Dadurch, dass /OE über den Pullup-Widerstand auf HIGH gezogen wurde,
befindet sich der 74HC595 im Tristate-Modus. Wird jetzt /OE auf LOW
gezogen, steht der gesetzte Zustand an den Ports an.
*/
//#define WITH_OE

// globales Array zur Speicherung der Bitmuster
extern unsigned char com74hc595[];

// Initialisierung der Ports im Mikrocontroller
extern void com74hc595_init(void);

// Ausgabe der Bitmuster an die 74HC595
extern void com74hc595_out(void);

// setzen eines einzelnen Bits innerhalb des Arrays
extern void com74hc595_setBit( unsigned char BitNumber );

// zurücksetzen eines einzelnen Bits innerhalb des Arrays
extern void com74hc595_unsetBit( unsigned char BitNumber );

// setzen eines ganzen Ports
extern void com74hc595_setPort( unsigned char Port, unsigned char Bits );

// zurücksetzen eines ganzen Ports
extern void com74hc595_unsetPort( unsigned char Port );

// setzt alle Bits auf logisch 1
extern void com74hc595_setall(void);

// setzt alle Bits auf logisch 0
extern void com74hc595_unsetall(void);

#endif /* SCHIEBEREGISTER_H_ */