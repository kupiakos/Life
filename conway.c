/*
 * conway.c
 *
 *  Created on: Nov 13, 2014
 *      Author: Kevin Haroldsen
 */


#include "conway.h"

// Each state is as large as I can make it. (Tested by experimentation)
sbyte state[STATESIZE];

// 9 bits for each map
// top 6 bits define index
// bottom 3 bits define where in the map (1 << n)
// map for each possible state. 0 = dead, 1 = alive
const uint8 knownstates[(1 << 9) >> 3] = {
		0x80, 0x68, 0xe8, 0x7e, 0x68, 0x16, 0x7e, 0x17,
		0x68, 0x16, 0x7e, 0x17, 0x16, 0x01, 0x17, 0x01,
		0x68, 0x16, 0x7e, 0x17, 0x16, 0x01, 0x17, 0x01,
		0x16, 0x01, 0x17, 0x01, 0x01, 0x00, 0x01, 0x00,
		0x68, 0x16, 0x7e, 0x17, 0x16, 0x01, 0x17, 0x01,
		0x16, 0x01, 0x17, 0x01, 0x01, 0x00, 0x01, 0x00,
		0x16, 0x01, 0x17, 0x01, 0x01, 0x00, 0x01, 0x00,
		0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

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
			draw_rle_pattern(50, 50, pulsar);
			break;
		default:
			switches = 0;
		}
	} while (switches == 0);
	switches = 0;
}

void cell_spawn(register uint8 row, register uint8 col)
{
	++row, ++col;
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
	draw_alive(row - 1, col - 1);
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

void draw_dead(register uint8 row, register uint8 col)
{
	lcd_point(col << 1, row << 1, 6);
	if (row == 0)
		lcd_rectangle(col << 1, 0, 2, 1, 1);
	else if (row == ROWS - 1)
		lcd_rectangle(col << 1, (ROWS << 1) - 1, 2, 1, 1);
	if (col == 0)
		lcd_rectangle(-1, row << 1, 1, 2, 1);
	else if (col == COLS - 1)
		lcd_rectangle((COLS << 1) - 1, row << 1, 1, 2, 1);
}

void init_system()
{
	RBX430_init(_16MHZ);
	lcd_init();
	lcd_volume(380);
	lcd_backlight(1);
	lcd_clear();
	watchdog_init();
	port1_init();
	P4DIR |= 0x40;
	__bis_SR_register(GIE);
}

void reset_simulation()
{
	WDT_Sec_Cnt = WDT_1SEC_CNT;
	lcd_clear();
	lcd_rectangle(0, 0, 160, 160, 1);
	thisgen = state;
	nextgen = state + GENSEP;
	*thisgen = 0;
	switches = 0;
	memset(state, 0, sizeof(state));
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
			y = *next + 1;
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

				if (bitmap_isalive(neighbors))
				{
					if (bitmap_fate(neighbors))
					{
						ip(new);
						*new = nx - 1;
					}
					else
						draw_dead(y - 1, -nx); // using next x
				}
				else if (bitmap_fate(neighbors))
				{
					// can use next x because drawing starts from 0.
					draw_alive(y - 1, -nx);
					ip(new);
					*new = nx - 1;
				}
				else if (!neighbors) // possibly move neighbors check down
					break;
				neighbors >>= 3;
			} while (nx++);
		} while (1);
	} while (1);

	*new = 0; // terminate our list

	next = nextgen;
	nextgen = state + ((nextgen - state) + GENSEP) % sizeof(state);
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
