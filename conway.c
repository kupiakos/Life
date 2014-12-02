/*
conway.c

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


#include "conway.h"

// Each state is as large as I can make it. (Tested by experimentation)
sbyte state[STATESIZE];

// Used to be highly compressed; 8 states per byte.
// Expanded to individual 1's and 0's to increase speed.

// 9 bits for each map
// map for each possible state. 0 = dead, 1 = alive
const uint8 knownstates[] = {
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0,
		0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0,
		0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0,
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint8 shiftleft3[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8,
		9, 9, 9, 9, 9, 9, 9, 9,
		10, 10, 10, 10, 10, 10, 10, 10,
		11, 11, 11, 11, 11, 11, 11, 11,
		12, 12, 12, 12, 12, 12, 12, 12,
		13, 13, 13, 13, 13, 13, 13, 13,
		14, 14, 14, 14, 14, 14, 14, 14,
		15, 15, 15, 15, 15, 15, 15, 15,
		16, 16, 16, 16, 16, 16, 16, 16,
		17, 17, 17, 17, 17, 17, 17, 17,
		18, 18, 18, 18, 18, 18, 18, 18,
		19, 19, 19, 19, 19, 19, 19, 19,
		20, 20, 20, 20, 20, 20, 20, 20,
		21, 21, 21, 21, 21, 21, 21, 21,
		22, 22, 22, 22, 22, 22, 22, 22,
		23, 23, 23, 23, 23, 23, 23, 23,
		24, 24, 24, 24, 24, 24, 24, 24,
		25, 25, 25, 25, 25, 25, 25, 25,
		26, 26, 26, 26, 26, 26, 26, 26,
		27, 27, 27, 27, 27, 27, 27, 27,
		28, 28, 28, 28, 28, 28, 28, 28,
		29, 29, 29, 29, 29, 29, 29, 29,
		30, 30, 30, 30, 30, 30, 30, 30,
		31, 31, 31, 31, 31, 31, 31, 31,
		32, 32, 32, 32, 32, 32, 32, 32,
		33, 33, 33, 33, 33, 33, 33, 33,
		34, 34, 34, 34, 34, 34, 34, 34,
		35, 35, 35, 35, 35, 35, 35, 35,
		36, 36, 36, 36, 36, 36, 36, 36,
		37, 37, 37, 37, 37, 37, 37, 37,
		38, 38, 38, 38, 38, 38, 38, 38,
		39, 39, 39, 39, 39, 39, 39, 39,
		40, 40, 40, 40, 40, 40, 40, 40,
		41, 41, 41, 41, 41, 41, 41, 41,
		42, 42, 42, 42, 42, 42, 42, 42,
		43, 43, 43, 43, 43, 43, 43, 43,
		44, 44, 44, 44, 44, 44, 44, 44,
		45, 45, 45, 45, 45, 45, 45, 45,
		46, 46, 46, 46, 46, 46, 46, 46,
		47, 47, 47, 47, 47, 47, 47, 47,
		48, 48, 48, 48, 48, 48, 48, 48,
		49, 49, 49, 49, 49, 49, 49, 49,
		50, 50, 50, 50, 50, 50, 50, 50,
		51, 51, 51, 51, 51, 51, 51, 51,
		52, 52, 52, 52, 52, 52, 52, 52,
		53, 53, 53, 53, 53, 53, 53, 53,
		54, 54, 54, 54, 54, 54, 54, 54,
		55, 55, 55, 55, 55, 55, 55, 55,
		56, 56, 56, 56, 56, 56, 56, 56,
		57, 57, 57, 57, 57, 57, 57, 57,
		58, 58, 58, 58, 58, 58, 58, 58,
		59, 59, 59, 59, 59, 59, 59, 59,
		60, 60, 60, 60, 60, 60, 60, 60,
		61, 61, 61, 61, 61, 61, 61, 61,
		62, 62, 62, 62, 62, 62, 62, 62,
		63, 63, 63, 63, 63, 63, 63, 63
};

int generation;
sbyte *thisgen, *nextgen; // Switch between state1 and state2.


void draw_rle_pattern(register int row, register int col, register const uint8* object)
{
	register uint8 c;
	int w = 0, h = 0, ocol = col;
	object += 4; // skip to the first #
	while ((c=*object++) && c != ',')
	{
		w *= 10;
		w += c - '0';
	}
	object += 5; // skip to the next #
	while ((c=*object++) && c != ',')
	{
		h *= 10;
		h += c - '0';
	}
	row += h - 1; // flip the image vertically
	// skip to the start
	object += 15;
	register uint16 repeat = 1;
	while ((c = *object++) && c != '!')
	{
		switch (c)
		{
		case 'b':
			col += repeat;
			repeat = 1;
			break;
		case 'o':
			while (repeat --> 0)
				cell_spawn(row, col++);
			repeat = 1;
			break;
		case '$':
			col = ocol;
			row -= repeat;
			break;
		default: // a number
			repeat = 0;
			do
			{
				repeat *= 10;
				repeat += c - '0';
			} while (isdigit(*object) && (c = *object++));
			break;
		}
	}

	return;
}

void choose_simulation()
{
	int i;
	do
	{
		while (switches == 0);
		lcd_clear();
		lcd_rectangle(0, 0, 160, 160, 1);
		switch (switches)
		{
		case LIFE:
			draw_rle_pattern(65, 30, gosper_glider_gun);
			draw_rle_pattern(60, 5, pulsar);
			draw_rle_pattern(4, 7, beacon_maker);
			draw_rle_pattern(4, 30, hexapole);
			draw_rle_pattern(4, 40, hexapole);
			draw_rle_pattern(4, 50, hexapole);
			draw_rle_pattern(4, 60, hexapole);
			draw_rle_pattern(15, 65, loafer);
			break;
		case BIRD:
			draw_rle_pattern(60, 5, pulsar);            // sun
			draw_rle_pattern(31, 2, bird);              // bird
			draw_rle_pattern(40, 20, hwss);             // bigger bird
			draw_rle_pattern(11, 29, glider);           // blows bird apart
			draw_rle_pattern(65, 65, loafer);

			for (i = 2; i < 78; i += 3)                 // draw ground
			{
			   draw_rle_pattern(7, i, block);           // block
			}
			break;
		case BOMB:
			draw_rle_pattern(65, 10, gosper_glider_gun);
			draw_rle_pattern(40, 10, lwss);             // LWSS

			draw_rle_pattern(20, 10, pulsar);           // pulsar
			draw_rle_pattern(20, 35, pulsar);           // pulsar
			draw_rle_pattern(20, 60, pulsar);           // pulsar

			draw_rle_pattern(10, 10, block);            // block
			draw_rle_pattern(10, 18, beehive);          // beehive
			draw_rle_pattern(10, 26, loaf);             // loaf
			draw_rle_pattern(10, 34, boat);             // boat
			draw_rle_pattern(10, 42, toad);             // toad
			draw_rle_pattern(10, 50, beacon);           // beacon
			draw_rle_pattern(10, 58, blinker);          // blinker
			draw_rle_pattern(10, 66, by_flop);          // by_flop
			break;
		case MINE:
			draw_rle_pattern(20, 20, glider1);
			draw_rle_pattern(20, 50, canada_goose);
			break;
		default:
			switches = 0;
		}
	} while (switches == 0);
	switches = 0;
}

void cell_spawn(register uint8 row, register uint8 col)
{
	register sbyte *p;
	// Find the correct group
	for (p = thisgen; *p < 0 || *p > row; ++p);
	if (*p == row)
	{
		register sbyte repl1, repl2;

		while (-(*++p) > col);
		if (*p == -col)
			return; // Nothing to do if it already exists.
		repl1 = -col;

		do
		{
			repl2 = *p;
			*p = repl1;
			repl1 = repl2;
		} while (*p++);
	}
	else
	{
		register sbyte new1, new2, repl1, repl2, *n;
		int cont = 0;
		new1 = row;
		new2 = -col;
		n = p;
		do
		{
			repl1 = *n++;
			repl2 = *n++;
			*p++ = new1;
			*p++ = new2;
			cont = new1 && new2;
			new1 = repl1;
			new2 = repl2;
		} while (cont);
	}
	draw_alive(row, col);
}

void cell_kill(register uint8 row, register uint8 col)
{
	return; // It seems right now that this might be unused.
	/*
	register sbyte *p, *c, *n;
	// Find the correct group
	for (p = thisgen; *p < 0 || *p > row; ++p);
	if (*p == row)
	{
		while (-(*++p) > col);
		if (*p == col)
		{
			draw_dead(row, col);
			n = p + 1;
			c = p;
			while (*c++ = *n++); // shift things ahead backward
			// delete the row if needed?
		}
	}*/
}

void init_system()
{
	RBX430_init(_16MHZ);
	lcd_init();
	lcd_volume(355);
	lcd_volume(358);
	lcd_backlight(1);
	lcd_clear();
	watchdog_init();
	port1_init();
	P4DIR |= 0x40;
	__bis_SR_register(GIE);
	switches = 0;
}

void reset_simulation()
{
	WDT_Sec_Cnt = WDT_1SEC_CNT;
	thisgen = state;
	nextgen = state + GENSEP;
	*thisgen = 0;
	memset(state, 0, sizeof(state));
	show_splash();
}

void init_simulation()
{
	seconds = 0;
	generation = 0;
}

void step_simulation()
{
	register sbyte *cur = thisgen, *new = nextgen;
	register sbyte *prev, *next;
	// a bitmap of everything around a cell, including itself.
	register uint16 neighbors = 0;
	// next x, current y
	register sbyte nx, y;

	prev = next = cur;

	*new = 0;

	// Note this algorithm will go from y = max y to 0, and x = max x to 0.
	do
	{
		// was an x coordinate written?
		if (*new < 0)
			ip(new);

		if (prev == next)
		{
			// start a new row group
			if (*next == 0)
				break;
			y = (*next) + 1;
			ip(next);
		}
		else
		{
			// move to next row and see which to scan
			if (*prev == y--)
				ip(prev);
			if (*cur == y)
				ip(cur);
			if (*next == y - 1)
				if (y - 1)
					ip(next);
		}

		// write the new y coordinate
		*new = y;
		do
		{
			nx = *prev;
			if (nx > *cur)
				nx = *cur;
			if (nx > *next)
				nx = *next;

			// any more x values to parse?
			if (nx >= 0)
				break;

			neighbors = 0;
			do
			{
				if (nx)
				{
					// add the next column to the neighbors bitmap
					if (*next == nx)
					{
						neighbors |= 0x100;
						ip(next);
					}
					if (*cur == nx)
					{
						neighbors |= 0x80;
						ip(cur);
					}
					if (*prev == nx)
					{
						neighbors |= 0x40;
						ip(prev);
					}
				}



				if (nx >= -(COLS - 1) && y <= ROWS && y > 0)
				{
					if (bitmap_isalive(neighbors))
					{
						if (bitmap_fate(neighbors))
						{
							ip(new);
							*new = nx - 1;
						}
						else
							draw_dead(y, -(nx - 1)); // using next x
					}
					else if (bitmap_fate(neighbors))
					{
						draw_alive(y, -(nx - 1));
						ip(new);
						*new = nx - 1;
					}
				}
				if (!(neighbors = shiftleft3[neighbors]))
					break;
			} while (nx++);
		} while (1);
	} while (1);

	*new = 0; // terminate our list

	next = nextgen;
	// nextgen = state + ((nextgen - state) + GENSEP) % sizeof(state);
	nextgen = (sbyte*)((nextgen - state) + GENSEP);
	if (!((uint16)nextgen < sizeof(state)))
		nextgen -= sizeof(state);
	nextgen += (uint16)state;
	thisgen = next;
}

void main(void)
{
	init_system();

	while (1)
	{
		reset_simulation();
		choose_simulation();
		init_simulation();
		do
		{
			step_simulation();
			P4OUT ^= 0x40;
		} while (!display_results(++generation));
	}
}


// END OF LINE.
