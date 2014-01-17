/*
 * Gleise.h
 *
 * Created: 25.12.2013 11:31:14
 *  Author: Christian
 */


#ifndef GLEISE_H_
#define GLEISE_H_

// Gleise (Eingänge)
#define ANZAHL_EINGANGSGLEISE 6

#define E_PORT PORTC
#define E_PIN PINC
#define E_DDR DDRC

#define EGHS_PIN 0
#define EGL_PIN 1
#define EGM_PIN 2
#define EGR_PIN 3
#define EGG_PIN 4
#define EGES_PIN 5

#define ZUG(X) !(E_PIN & (1 << X))

// Gleise (Ausgänge)
#define GL_PIN 7
#define GM_PIN 6
#define GR_PIN 5
#define GG_PIN 4

#define WL_PIN 3
#define WR_PIN 2

#define HSRED_PIN 1
#define HSGREEN_PIN 0

#define GL_ON com74hc595_setBit(GL_PIN)
#define GM_ON com74hc595_setBit(GM_PIN)
#define GR_ON com74hc595_setBit(GR_PIN)
#define GG_ON com74hc595_setBit(GG_PIN)

#define GL_OFF com74hc595_unsetBit(GL_PIN)
#define GM_OFF com74hc595_unsetBit(GM_PIN)
#define GR_OFF com74hc595_unsetBit(GR_PIN)
#define GG_OFF com74hc595_unsetBit(GG_PIN)

// Weiche
#define WL_ON com74hc595_setBit(WL_PIN)
#define WR_ON com74hc595_setBit(WR_PIN)

#define WL_OFF com74hc595_unsetBit(WL_PIN)
#define WR_OFF com74hc595_unsetBit(WR_PIN)

// Hauptsignal
#define HSRED_ON com74hc595_setBit(HSRED_PIN)//HS_PORT &=~ (1 << HSRED_PIN)
#define HSRED_OFF com74hc595_unsetBit(HSRED_PIN)//HS_PORT |= (1 << HSRED_PIN)
#define HSGREEN_ON com74hc595_setBit(HSGREEN_PIN)//HS_PORT &=~ (1 << HSGREEN_PIN)
#define HSGREEN_OFF com74hc595_unsetBit(HSGREEN_PIN)//HS_PORT |= (1 << HSGREEN_PIN)

extern void glOn (void);
extern void glOff (void);
extern void gmOn (void);
extern void gmOff (void);
extern void grOn (void);
extern void grOff (void);
extern void ggOn (void);
extern void ggOff (void);
extern void weicheLinks (void);
extern void weicheRechts (void);
extern void hauptsignalRed (void);
extern void hauptsignalGreen (void);

#endif /* GLEISE_H_ */