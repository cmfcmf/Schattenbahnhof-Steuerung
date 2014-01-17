/*
 * Gleise.c
 *
 * Created: 25.12.2013 11:30:54
 *  Author: Christian
 */ 

#include <avr/io.h>

#include "global.h"
#include "Outputs.h"
#include "SBHFSTRGv2.h"
#include "Schieberegister.h"

extern weicheType weiche;
extern hauptsignalType hauptsignal;

inline void glOn (void) {
	GL_ON;
}

inline void glOff (void) {
	GL_OFF;
}

inline void gmOn (void) {
	GM_ON;
}

inline void gmOff (void) {
	GM_OFF;
}

inline void grOn (void) {
	GR_ON;
}

inline void grOff (void) {
	GR_OFF;
}

inline void ggOn (void) {
	GG_ON;
}

inline void ggOff (void) {
	GG_OFF;
}

inline void weicheLinks (void) {
	WR_OFF;
	WL_ON;
	weiche.status = Links;
	weiche.counter = 0;
}

inline void weicheRechts (void) {
	WL_OFF;
	WR_ON;
	weiche.status = Rechts;
	weiche.counter = 0;
}

inline void hauptsignalRed (void) {
	HSGREEN_OFF;
	HSRED_ON;
	hauptsignal.status = Red;
	hauptsignal.counter = 0;
}

inline void hauptsignalGreen (void) {
	HSRED_OFF;
	HSGREEN_ON;
	hauptsignal.status = Green;
	hauptsignal.counter = 0;
}