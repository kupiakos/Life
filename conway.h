/*
 * conway.h
 *
 *  Created on: Nov 13, 2014
 *      Author: Kevin Haroldsen
 */

#ifndef CONWAY_H_
#define CONWAY_H_

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "msp430.h"
#include "RBX430-1.h"
#include "RBX430_lcd.h"
#include "lifelib.h"

#define sbyte signed char

#define ROWS	80
#define COLS	80
#define COLSECTS	10

#define STATESIZE	880
#define GENSEP	STATESIZE-(ROWS*3+4)
#define ip(x) if (++x >= state + sizeof(state)) x -= sizeof(state)

void init_system();
void reset_simulation();
void init_simulation();
void step_simulation();
void choose_simulation();

void cell_kill(register uint8 row, register uint8 col);
void cell_spawn(register uint8 row, register uint8 col);

void draw_rle_pattern(register int row, register int col, register const uint8* object);
enum SEED { LIFE=0x01, BIRD=0x02, BOMB=0x04, MINE=0x08 };

#define draw_alive(y, x) lcd_point(x << 1, y << 1, 7)
void draw_dead(register uint8 row, register uint8 col);

#define write_mem(address, datum) FRAM_write((uint16)address, (uint8)datum);
#define read_mem(address) (int8)FRAM_read((uint16)address);

#define cell_alive(row, col) (universe[row][col >> COLSPOW] & (0x80 >> (col & COLSPOWSEL)))

#define bitmap_isalive(x) (x & 0x10)
#define bitmap_fate(x) (knownstates[x >> 3] & (1 << (x & 7)))
#define bitmap_mapalive(i) knownstates[i >> 3] |= (1 << (i & 7))
#define bitmap_mapdead(i) knownstates[i >> 3] &= ~(1 << (i & 7))

#endif /* CONWAY_H_ */
