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
#include "RBX430_i2c.h"
#include "lifelib.h"

#define ROWS	80
#define COLS	80
#define GEN1_START	(sbyte*)0x0000;
#define GEN2_START	(sbyte*)0x4000;

#define sbyte signed char

#define waitswitch();	while (switches == 0); switches = 0;

void init_system();
void reset_simulation();
void gen_map();
void init_simulation();
void step_simulation();
void choose_simulation();

void cell_kill(uint8 row, uint8 col);
inline void cell_spawn(uint8 row, uint8 col);

void draw_rle_pattern(int row, int col, const uint8* object);
enum SEED { LIFE=0x01, BIRD=0x02, BOMB=0x04, MINE=0x08 };

#define draw_alive(y, x) lcd_point(x << 1, y << 1, 7)
void draw_dead(uint8 row, uint8 col);

#define write_mem(address, datum) FRAM_write((uint16)address, (uint8)datum);
#define read_mem(address) (int8)FRAM_read((uint16)address);

#define cell_alive(row, col) (universe[row][col >> COLSPOW] & (0x80 >> (col & COLSPOWSEL)))

#define bitmap_isalive(x) (x & 0x10)
#define bitmap_fate(x) (knownstates[x >> 3] & (1 << (x & 7)))
#define bitmap_mapalive(i) knownstates[i >> 3] |= (1 << (i & 7))
#define bitmap_mapdead(i) knownstates[i >> 3] &= ~(1 << (i & 7))

#endif /* CONWAY_H_ */
