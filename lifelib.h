/*
 * lifelib.h
 *
 *  Created on: Nov 13, 2014
 *      Author: root
 */

#ifndef LIFELIB_H_
#define LIFELIB_H_

// includes
#include <stdlib.h>
#include <string.h>
#include "msp430.h"
#include "RBX430-1.h"
#include "RBX430_lcd.h"

// global variables
extern volatile uint16 WDT_Sec_Cnt;			// WDT second counter
extern volatile uint16 WDT_debounce_cnt;		// WDT debounce counter
extern volatile uint16 seconds;				// # of seconds
extern volatile uint16 switches;				// debounced switch values

#ifndef MAX_TIME
#define MAX_TIME		60
#endif

// defined constants -----------------------------------------------------------
#define myCLOCK			16000000		// 16 MHz
#define	WDT_CTL			WDT_MDLY_32		// WD configuration (SMCLK, ~32 ms)
#define WDT_CPI			32000			// WDT Clocks Per Interrupt (@1 Mhz)
#define	WDT_1SEC_CNT	myCLOCK/WDT_CPI	// WDT counts/second

#define MAX_TIME		60
#define DEBOUNCE_CNT	2

int display_results(uint16 generation);
void watchdog_init(void);
void port1_init(void);

// still lifes
extern const uint8 block[];
extern const uint8 beehive[];
extern const uint8 loaf[];
extern const uint8 boat[];

// oscillators
extern const uint8 blinker[];
extern const uint8 toad[];
extern const uint8 beacon[];
extern const uint8 by_flop[];
extern const uint8 hexapole[];
extern const uint8 pulsar[];

// guns
extern const uint8 gosper_glider_gun[];
extern const uint8 gosper_glider_gun2[];

// spaceships
extern const uint8 glider[];
extern const uint8 glider1[];
extern const uint8 lwss[];
extern const uint8 mwss[];
extern const uint8 hwss[];
extern const uint8 bird[];
extern const uint8 loafer[];
extern const uint8 canada_goose[];

// factories
extern const uint8 beacon_maker[];
extern const uint8 blinker_puffer[];
extern const uint8 carnivore[];
extern const uint8 converter[];

extern const uint8 jon_arbuckle[];

// Highlife patterns
extern const uint8 replicator[];

// lcd images
extern const uint16 life_image[];

#endif /* LIFELIB_H_ */
