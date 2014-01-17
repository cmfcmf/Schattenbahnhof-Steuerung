/*
 * global.h
 *
 * Created: 24.12.2013 18:50:59
 *  Author: Christian
 */


#ifndef GLOBAL_H_
#define GLOBAL_H_

// Rundenzähler
#define RUNDENTIMER_OVERFLOW_VALUE 256
#define RUNDENTIMER_PRESCALER 64
#define WARTEZEIT_PRO_RUNDE (1000 / (F_CPU / RUNDENTIMER_OVERFLOW_VALUE / RUNDENTIMER_PRESCALER))

// On / Off Schalter
#define ONOFF_PORT PORTD
#define ONOFF_DDR DDRD
#define ONOFF_PIN PIND
#define ON_OFF_BIT PD2

// Weiche
#define SCHALTZEIT_WEICHE 200
#define SCHALTZEIT_WEICHE_COUNTER (SCHALTZEIT_WEICHE / WARTEZEIT_PRO_RUNDE)

#if SCHALTZEIT_WEICHE_COUNTER > 250
#error SCHALTZEIT_WEICHE zu gross oder WARTEZEIT_PRO_RUNDE zu klein!
#endif
#if SCHALTZEIT_WEICHE_COUNTER == 0
#error SCHALTZEIT_WEICHE zu klein oder WARTEZEIT_PRO_RUNDE zu gross!
#endif

// Entprellung
#define ENTPRELLZEIT 5000
#define ENTPRELL_COUNTER (ENTPRELLZEIT / WARTEZEIT_PRO_RUNDE)

#if ENTPRELL_COUNTER > 65500
#error ENTPRELLZEIT zu gross oder WARTEZEIT_PRO_RUNDE zu klein!
#endif
#if ENTPRELL_COUNTER == 0
#error ENTPRELLZEIT zu klein oder WARTEZEIT_PRO_RUNDE zu gross!
#endif

/*
// Hauptsignal Ausgang
#define HS_PORT PORTB
#define HS_DDR DDRB

#define HSRED_PIN PB0
#define HSGREEN_PIN PB1*/

#define SCHALTZEIT_HS 500
#define SCHALTZEIT_HS_COUNTER (SCHALTZEIT_HS / WARTEZEIT_PRO_RUNDE)

#if SCHALTZEIT_HS_COUNTER > 250
#error SCHALTZEIT_HS zu gross oder WARTEZEIT_PRO_RUNDE zu klein!
#endif
#if SCHALTZEIT_HS_COUNTER == 0
#error SCHALTZEIT_HS zu klein oder WARTEZEIT_PRO_RUNDE zu gross!
#endif

// Debug LED
#ifdef DEBUG
	#define DEBUG_LED_PIN PD3
	#define DEBUG_LED_PORT PORTD
	#define DEBUG_LED_DDR DDRD

	#define debugLedToggle() DEBUG_LED_PORT ^= (1 << DEBUG_LED_PIN)
	#define debugLedOn() DEBUG_LED_PORT |= (1 << DEBUG_LED_PIN)
	#define debugLedOff() DEBUG_LED_PORT &=~ (1 << DEBUG_LED_PIN)
#endif

#endif /* GLOBAL_H_ */