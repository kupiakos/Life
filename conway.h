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

#define ROWS	80
#define COLS	80
#define COLSTYPE	uint8
#define COLSSIZE	8
#define COLSPOW 	(COLSSIZE == 8 ? 3 : 4)
#define COLSECTS	(COLS/COLSSIZE)
#define COLSLEFT	(1 << (COLSSIZE-1))
#define COLSLEFT2	(3 << (COLSSIZE-2))
#define COLS2NDLEFT	(1 << (COLSSIZE-2))
#define COLSPOWSEL	((1 << (COLSPOW)) - 1)

#define waitswitch();	while (switches == 0); switches = 0;

void init_system();
void reset_simulation();
void step_simulation();
void choose_simulation();
uint8 number_bits(COLSTYPE number);

void cell_kill(uint8 row, uint8 col);
void cell_spawn(uint8 row, uint8 col);

void draw_rle_pattern(int row, int col, const uint8* object);
enum SEED { LIFE=0x01, BIRD=0x02, BOMB=0x04, MINE=0x08 };

#define draw_alive(y, x) lcd_point(x << 1, y << 1, 7)
#define draw_dead(y, x) lcd_point(x << 1, y << 1, 6)

#define cell_alive(row, col) (universe[row][col >> COLSPOW] & (0x80 >> (col & COLSPOWSEL)))

#endif /* CONWAY_H_ */
