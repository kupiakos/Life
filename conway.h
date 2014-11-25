/*
conway.h

Created on: Nov 13, 2014
    Author: Kevin Haroldsen

I certify this file is of my own creation and effort.

The MIT License (MIT)

Copyright (c) 2014 Kevin Haroldsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

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

#define ROWS	78
#define COLS	78
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
#define bitmap_fate(x) (knownstates[x])

#endif /* CONWAY_H_ */
