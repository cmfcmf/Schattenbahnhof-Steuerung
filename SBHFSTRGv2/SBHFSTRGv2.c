/*
 * SBHFSTRGv2.c
 *
 * Created: 24.12.2013 18:50:07
 *  Author: Christian
 */

#include <avr/io.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "global.h"
#include "Schieberegister.h"
#include "Outputs.h"
#include "SBHFSTRGv2.h"

#define STEUERUNG1 1
#define STEUERUNG2 2
#define STEUERUNG3 3

#define delay_ms(X) _delay_ms(X)
//#define delay_ms(X)

// Struktur für ein Eingangsgleis.
volatile gleis gleise[ANZAHL_EINGANGSGLEISE];

volatile weicheType weiche;
volatile hauptsignalType hauptsignal;

steuerungStatus status[3];

/**
 * The falling edge of INT0 generates an interrupt request.
 */
ISR(INT0_vect) {
	// Schattenbahnhofsteuerung inaktiv.
	com74hc595_unsetall();
	hauptsignalRed();
	com74hc595_out();
	wdt_enable(WDTO_250MS);
	while (1) {
		// Do nothing.
	}
}

static void readTracks (void) {
	for (uint8_t i = 0; i < ANZAHL_EINGANGSGLEISE; i++) {
		bool zug;
		zug = ZUG(i);

		if (zug) {
			gleise[i].counter = 0;
			gleise[i].current = true;
		} else if (gleise[i].counter < ENTPRELL_COUNTER) {
			// Kein Zug, aber letzter Zug noch < 5000ms her.
			gleise[i].current = true;
		} else {
			// Kein Zug, und letzter Zug schon >= 5000ms her.
			gleise[i].current = false;
		}
	}
	/*
	// Eingangsoptokoppler
	for (uint8_t i = 0; i < ANZAHL_EINGANGSGLEISE; i++) {
		bool zug;
		zug = ZUG(i);

		if (zug == false && gleise[i].old == true && gleise[i].counter < ENTPRELL_COUNTER) {
			gleise[i].current = true;
			} else {
			gleise[i].counter = 0;
			gleise[i].current = ZUG(i);
		}
	}
	*/
}

/**
 * Called every 1000 / (F_CPU / 64 / 256) ms. For F_CPU = 8000000 this means every 2 ms.
 */
ISR(TIMER0_OVF_vect) {
	// Erhöhe Optokoppler Eingangszähler
	for (uint8_t i = 0; i < ANZAHL_EINGANGSGLEISE; i++) {
		if (gleise[i].counter < ENTPRELL_COUNTER) {
			gleise[i].counter++;
		}
	}

	// Erhöhe Weichenzähler
	weiche.counter++;

	// Erhöhe Hauptsignalzähler
	hauptsignal.counter++;

	debugLedToggle();
}

/**
 * Read random seed from eeprom and write a new random one.
 */
static inline void initrand()
{
    uint32_t state;
    static uint32_t EEMEM sstate;

    state = eeprom_read_dword(&sstate);

    // Check if it's unwritten EEPROM (first time).
    if (state == 0xffffffUL) {
        state = 0;
	}

    srandom(state);
    eeprom_write_dword(&sstate, state++);
}

/**
 * Initialise everthing.
 */
static inline void init (void) {
	// Watchdog deaktivieren
	wdt_disable();

	// Schieberegister
	com74hc595_init();

	// Eingänge
	E_PORT = 0xFF; // Pullups

	// Reset der Gleiseingänge
	for (uint8_t i = 0; i < ANZAHL_EINGANGSGLEISE; i++) {
		gleise[i].counter = ENTPRELL_COUNTER;
		gleise[i].current = false;
		gleise[i].old = false;
	}

	// Reset der Weiche
	weiche.status = Pause;
	weiche.counter = 0;

	// Reset des Hauptsignals
	hauptsignal.status = Aus;
	hauptsignal.counter = 0;

	/*
	// Hauptsignal
	HS_DDR |= (1 << HSRED_PIN) | (1 << HSGREEN_PIN);
	HSGREEN_OFF;
	HSRED_OFF;
*/
    // Steuerung
    for (uint8_t i = 0; i < 3; i++) {
        status[i].action = WAITING_FOR_NEW_TRAIN;
        status[i].start = -1;
        status[i].ziel = -1;
    }

    initrand();

	// Externer Interrupt
	MCUCR |= (1 << ISC01); // The falling edge of INT0 generates an interrupt request
	GICR |= (1 << INT0);   // Enable interrupt

	// On / Off Schalter
	ONOFF_PORT |= (1 << ON_OFF_BIT);

	// Rundentimer
	TCCR0 |= (1 << CS01) | (1 << CS00); // Prescaler 64
	TIMSK |= (1 << TOIE0); // Interrupt enable
	// Interrupt also alle 1000 / (F_CPU / 64 / 256) ms. Bei F_CPU = 8000000 alle 2 ms.

	// Do not enable interrupts now!!
}

int8_t chooseAbschnitt(bool abschnitte[], uint8_t len, uint8_t strategie, uint8_t id, uint8_t startOrDestination) {
	switch (strategie) {
		case STRATEGIE_RANDOM:
		{
			uint8_t counter = 0;
			for (uint8_t i = 0; i < len; i++) {
				if (abschnitte[i]) {
					counter++;
				}
			}
			if (counter == 0) {
				return -1;
			}

			uint8_t chosen = random() % counter;

			counter = 0;
			for (uint8_t i = 0; i < len; i++) {
				if (abschnitte[i]) {
					if (chosen == counter) {
						return i;
					}
					counter++;
				}
			}
			break;
		}
		case STRATEGIE_LEFTTORIGHT:
		case STRATEGIE_FORCELEFTTORIGHT:
		{
			static int8_t chosenBefore[3][2] = {{-1, -1}, {-1, -1}, {-1, -1}};

			for (uint8_t i = 0; i < len; i++) {
				if (abschnitte[i] && i > chosenBefore[id][startOrDestination]) {
					chosenBefore[id][startOrDestination] = i;
					if (chosenBefore[id][startOrDestination] == len - 1) {
						chosenBefore[id][startOrDestination] = -1;
					}
					return i;
				}
			}
			for (int8_t i = len - 1; i >= 0 && strategie != STRATEGIE_FORCELEFTTORIGHT; i--) {
				if (abschnitte[i]) {
					chosenBefore[id][startOrDestination] = i;
					if (chosenBefore[id][startOrDestination] == len - 1) {
						chosenBefore[id][startOrDestination] = -1;
					}
					return i;
				}
			}
			break;
		}
		case STRATEGIE_RIGHTTOLEFT:
		case STRATEGIE_FORCERIGHTTOLEFT:
		{
			static int8_t chosenBefore[3][2] = {{100, 100}, {100, 100}, {100, 100}};

			for (int8_t i = len - 1; i >= 0; i--) {
				if (abschnitte[i] && i < chosenBefore[id][startOrDestination]) {
					chosenBefore[id][startOrDestination] = i;
					return i;
				}
			}
			for (uint8_t i = 0; i < len && strategie != STRATEGIE_FORCERIGHTTOLEFT; i++) {
				if (abschnitte[i]) {
					chosenBefore[id][startOrDestination] = i;
					return i;
				}
			}
			chosenBefore[id][startOrDestination] = 100;
			break;
		}
		case STRATEGIE_ASRIGHTASPOSSIBLE:
			for (int8_t i = len - 1; i >= 0; i--) {
				if (abschnitte[i]) {
					return i;
				}
			}
			break;
		case STRATEGIE_ASLEFTASPOSSIBLE:
		case STRATEGIE_DEFAULT:
		default:
			for (uint8_t i = 0; i < len; i++) {
				if (abschnitte[i]) {
					return i;
				}
			}
			break;
	}

	return -1;
}

int8_t belegterStartAbschnitt(abschnitt startAbschnitte[], uint8_t startLen, uint8_t startStrategie, uint8_t id) {
    bool belegteStartAbschnitte[startLen];

    for (uint8_t start = 0; start < startLen; start++) {
	    belegteStartAbschnitte[start] = *startAbschnitte[start].belegt;
    }

    return chooseAbschnitt(belegteStartAbschnitte, startLen, startStrategie, id, 0);
}

int8_t freierZielAbschnitt(abschnitt zielAbschnitte[], uint8_t zielLen, uint8_t zielStrategie, uint8_t id) {
    bool freieZielAbschnitte[zielLen];

    for (uint8_t ziel = 0; ziel < zielLen; ziel++) {
		freieZielAbschnitte[ziel] = !(*zielAbschnitte[ziel].belegt);
    }

    return chooseAbschnitt(freieZielAbschnitte, zielLen, zielStrategie, id, 1);
}

#ifdef DEBUG
void debug (uint8_t z) {return;
	debugLedOff();
	delay_ms(200);
	debugLedOn();
	delay_ms(2000);
	debugLedOff();
	delay_ms(500);
	for (uint8_t i = 0; i < z; i++) {
		debugLedOn();
		delay_ms(500);
		debugLedOff();
		delay_ms(500);
	}


	debugLedOff();
	delay_ms(2000);
}
#endif

void steuerung(abschnitt startAbschnitte[], const uint8_t startLen, const uint8_t startStrategie, abschnitt zielAbschnitte[], const uint8_t zielLen, const uint8_t zielStrategie, const uint8_t id) {
    switch (status[id].action) {
        case WAITING_FOR_NEW_TRAIN:
		{
			int8_t start = belegterStartAbschnitt(startAbschnitte, startLen, startStrategie, id);
			status[id].start = start;
            if (start == -1) {
                break;
            } else {
				status[id].ziel = -1;
                status[id].action = WAITING_FOR_EMPTY_DESTINATION;
            }
		}
        case WAITING_FOR_EMPTY_DESTINATION:
        {
			if (status[id].start == -1) {
				debug(1);
				status[id].action = WAITING_FOR_NEW_TRAIN; // ERROR!!!
				break;
			}

            int8_t ziel = freierZielAbschnitt(zielAbschnitte, zielLen, zielStrategie, id);
			if (ziel == -1) {
		        break;
		    } else {
				if (zielAbschnitte[ziel].bevorFahrt != NULL) {
					zielAbschnitte[ziel].bevorFahrt();
				}
				startAbschnitte[(status[id].start)].an();
                status[id].ziel = ziel;
		        status[id].action = WAITING_FOR_LEAVING_START;
	        }
        }
        case WAITING_FOR_LEAVING_START:
			if (*zielAbschnitte[(status[id].ziel)].belegt) {
				debug(2);
				startAbschnitte[(status[id].start)].aus();
				status[id].action = WAITING_FOR_NEW_TRAIN; // ERROR!!!
				break;
			}
            if (!*startAbschnitte[(status[id].start)].belegt) {
	            startAbschnitte[(status[id].start)].aus();
                status[id].action = WAITING_FOR_ARRIVAL_AT_DESTINATION;
            }
            break;
        case WAITING_FOR_ARRIVAL_AT_DESTINATION:
            if (*zielAbschnitte[(status[id].ziel)].belegt) {
	            status[id].action = WAITING_FOR_NEW_TRAIN;
            }
            break;
        case ERROR:
            break;
    }
}

int main (void) {
	// Debug
	#ifdef DEBUG
		DEBUG_LED_DDR |= (1 << DEBUG_LED_PIN);
	#endif

	init();

	if (!(ONOFF_PIN & (1 << ON_OFF_BIT))) {
		while (!(ONOFF_PIN & (1 << ON_OFF_BIT))) {
			// Warte bis Steuerung aktiviert wird.
		}
		// Wait 500ms, because the switch has to be debounced.
		delay_ms(500);
	}

	sei();

#ifdef STEUERUNG1
	abschnitt startAbschnitte1[1];
	startAbschnitte1[0].belegt     = &gleise[EGHS_PIN].current;
	startAbschnitte1[0].an         = hauptsignalGreen;
	startAbschnitte1[0].aus        = hauptsignalRed;
	startAbschnitte1[0].bevorFahrt = NULL; // Nicht nötig für einen Startabschnitt

	abschnitt zielAbschnitte1[2];
	zielAbschnitte1[0].belegt     = &gleise[EGL_PIN].current;
	zielAbschnitte1[0].an         = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte1[0].aus        = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte1[0].bevorFahrt = weicheLinks;
	zielAbschnitte1[1].belegt     = &gleise[EGM_PIN].current;
	zielAbschnitte1[1].an         = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte1[1].aus        = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte1[1].bevorFahrt = weicheRechts;
#endif
#ifdef STEUERUNG2
	abschnitt startAbschnitte2[3];
	startAbschnitte2[0].belegt     = &gleise[EGL_PIN].current;
	startAbschnitte2[0].an         = glOn;
	startAbschnitte2[0].aus        = glOff;
	startAbschnitte2[0].bevorFahrt = NULL; // Nicht nötig für einen Startabschnitt
	startAbschnitte2[1].belegt     = &gleise[EGM_PIN].current;
	startAbschnitte2[1].an         = gmOn;
	startAbschnitte2[1].aus        = gmOff;
	startAbschnitte2[1].bevorFahrt = NULL; // Nicht nötig für einen Startabschnitt
	startAbschnitte2[2].belegt     = &gleise[EGR_PIN].current;
	startAbschnitte2[2].an         = grOn;
	startAbschnitte2[2].aus        = grOff;
	startAbschnitte2[2].bevorFahrt = NULL; // Nicht nötig für einen Startabschnitt

	abschnitt zielAbschnitte2[1];
	zielAbschnitte2[0].belegt     = &gleise[EGG_PIN].current;
	zielAbschnitte2[0].an         = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte2[0].aus        = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte2[0].bevorFahrt = NULL;
#endif
#ifdef STEUERUNG3
	abschnitt startAbschnitte3[1];
	startAbschnitte3[0].belegt     = &gleise[EGG_PIN].current;
	startAbschnitte3[0].an         = ggOn;
	startAbschnitte3[0].aus        = ggOff;
	startAbschnitte3[0].bevorFahrt = NULL; // Nicht nötig für einen Startabschnitt

	abschnitt zielAbschnitte3[1];
	zielAbschnitte3[0].belegt     = &gleise[EGES_PIN].current;
	zielAbschnitte3[0].an         = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte3[0].aus        = NULL; // Nicht nötig für einen Zielabschnitt
	zielAbschnitte3[0].bevorFahrt = NULL;
#endif

	hauptsignalRed();

	while(1)
	{
		readTracks();

		// Weiche
		if (weiche.counter >= SCHALTZEIT_WEICHE_COUNTER) {
			weiche.counter = 0;
			weiche.status = Pause;
			WR_OFF;
			WL_OFF;
		}

		// Hauptsignal
		if (hauptsignal.counter >= SCHALTZEIT_HS_COUNTER) {
			hauptsignal.counter = 0;
			hauptsignal.status = Aus;
			HSRED_OFF;
			HSGREEN_OFF;
		}

#ifdef STEUERUNG1
        steuerung(startAbschnitte1, 1, STRATEGIE_DEFAULT, zielAbschnitte1, 2, STRATEGIE_RANDOM,  0);
#endif
#ifdef STEUERUNG2
		steuerung(startAbschnitte2, 3, STRATEGIE_RANDOM,  zielAbschnitte2, 1, STRATEGIE_DEFAULT, 1);
#endif
#ifdef STEUERUNG3
		steuerung(startAbschnitte3, 1, STRATEGIE_DEFAULT, zielAbschnitte3, 1, STRATEGIE_DEFAULT, 2);
#endif

		// Übernehme Schieberegister an Ausgänge.
		com74hc595_out();

		for (uint8_t i = 0; i < ANZAHL_EINGANGSGLEISE; i++) {
			gleise[i].old = gleise[i].current;
		}
	}
}