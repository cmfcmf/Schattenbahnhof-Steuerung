/*
 * SBHFSTRGv2.h
 *
 * Created: 25.12.2013 11:38:55
 *  Author: Christian
 */

#ifndef SBHFSTRGV2_H_
#define SBHFSTRGV2_H_

#include <stdbool.h>

#define STRATEGIE_DEFAULT           8
#define STRATEGIE_RANDOM            1
#define STRATEGIE_LEFTTORIGHT       2
#define STRATEGIE_RIGHTTOLEFT       3
#define STRATEGIE_ASLEFTASPOSSIBLE  4
#define STRATEGIE_ASRIGHTASPOSSIBLE 5
#define STRATEGIE_FORCELEFTTORIGHT  6
#define STRATEGIE_FORCERIGHTTOLEFT  7

// Function pointer to a "void func (void)" function.
typedef void (*VoidFnct) (void);

// Enum für den Weichenstatus
typedef enum {
	Pause,
	Links,
	Rechts
} weichenStatus;

// Enum für den Hauptsignalstatus
typedef enum {
	Aus,
	Red,
	Green
} hauptsignalStatus;

typedef struct {
	VoidFnct an;
	VoidFnct aus;
	VoidFnct bevorFahrt;
	volatile bool *belegt;
} abschnitt;

typedef enum {
	WAITING_FOR_NEW_TRAIN,
	WAITING_FOR_EMPTY_DESTINATION,
	WAITING_FOR_LEAVING_START,
	WAITING_FOR_ARRIVAL_AT_DESTINATION,
	ERROR
} steuerungAction;

typedef struct {
	steuerungAction action;
	int8_t start;
	int8_t ziel;
} steuerungStatus;

// Struktur für eine Weiche
typedef struct {
	volatile uint8_t counter;
	weichenStatus status;
} weicheType;

// Struktur für ein Hauptsignal
typedef struct {
	volatile uint8_t counter;
	hauptsignalStatus status;
} hauptsignalType;

// Struktur für ein Gleis.
typedef struct {
	volatile bool current;
	volatile bool old;
	volatile uint16_t counter;
} gleis;

#endif /* SBHFSTRGV2_H_ */