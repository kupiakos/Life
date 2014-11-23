/*
 * conway.c
 *
 *  Created on: Nov 13, 2014
 *      Author: Kevin Haroldsen
 */


#include "conway.h"

uint8 universe[ROWS][10];
// 9 bits for each map
// top 6 bits define index
// bottom 3 bits define where in the map (1 << n)
uint8 knownstates[(1 << 9) >> 3]; // map for each possible state. 0 = dead, 1 = alive
int generation;
sbyte *thisgen, *nextgen; // Actually stored in FRAM.


void draw_rle_pattern(int row, int col, const uint8* object)
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
	uint16 repeat = 1;
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
			draw_rle_pattern(74, 10, glider);
			draw_rle_pattern(40, 74, glider);
			break;
		default:
			switches = 0;
		}
	} while (switches == 0);
	switches = 0;
}

inline void cell_spawn(uint8 row, uint8 col)
{
	universe[row][col >> 3] |= (0x80 >> (col & 7));
	draw_alive(row, col);
}

void cell_kill(uint8 row, uint8 col)
{
	universe[row][col >> 3] &= ~(0x80 >> (col & 7));
	draw_dead(row, col);
}

void draw_dead(uint8 row, uint8 col)
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
	FRAM_init();
}

void reset_simulation()
{
	WDT_Sec_Cnt = WDT_1SEC_CNT;
	lcd_clear();
	lcd_rectangle(0, 0, 160, 160, 1);
}

void gen_map()
{
	register int n, c;
	register uint16 i = 1 << 9;
	while (i --> 0)
	{
		for (n = 0, c = i; c; ++n, c &= c - 1); // count bits
		if (bitmap_isalive(i))
		{
			if (n == 2 || n == 3)
				bitmap_mapalive(i);
			else
				bitmap_mapdead(i);
		}
		else
		{
			if (n == 3)
				bitmap_mapalive(i);
			else
				bitmap_mapdead(i);
		}
	}
}

void init_simulation()
{
	// Write the data in universe to FRAM.
	register int row = ROWS, colg;
	register sbyte val;
	thisgen = GEN1_START;
	nextgen = GEN2_START;

	while (row --> 0)
	{
		int empty = 1;
		colg = 10;
		write_mem(thisgen++, row);
		while (colg --> 0)
		{
			register char i, n;
			if (!(val = universe[row][colg]))
				continue;
			empty = 0;
			for (i=1, n=0; i < 8; ++n, i <<= 1)
				if (val & i)
					write_mem(thisgen++, -(((colg + 1) << 3) - n));
		}
		if (empty) --thisgen;
	}
	write_mem(thisgen, 0);
	thisgen = GEN1_START;

	seconds = 0;
	generation = 0;
}

void step_simulation()
{
	sbyte *cur = thisgen, *new = nextgen;
	sbyte *prev, *next;
	register sbyte cur_v, prev_v, next_v, new_v;
	register uint16 bitmap = 0;
	register sbyte x, y;

	cur_v = read_mem(cur);
	prev = next = cur;
	prev_v = next_v = cur_v;

	new_v = 0;
	write_mem(new, new_v);

	do
	{
		// was an x coordinate written?
		if (new_v < 0)
			new_v = read_mem(++new);

		if (prev == next)
		{
			// start a new row group
			if (next_v == 0)
			{
				write_mem(new, 0);
				break;
			}
			y = next_v + 1;
			next_v = read_mem(++next);
		}
		else
		{
			// move to next row and see which to scan
			if (prev_v == y--)
				prev_v = read_mem(++prev);
			if (cur_v == y)
				cur_v = read_mem(++cur);
			if (next_v == y - 1)
				next_v = read_mem(++next);
		}

		// write the new y coordinate
		write_mem(new, y);
		do
		{
			x = prev_v;
			if (x > cur_v)
				x = cur_v;
			if (x > next_v)
				x = next_v;

			// end of the row?
			if (x >= 0)
				break;

			do
			{
				// add the next column to the bitmap
				if (prev_v == x)
				{
					bitmap |= 0x40;
					prev_v = read_mem(++prev);
				}
				if (cur_v == x)
				{
					bitmap |= 0x80;
					cur_v = read_mem(++cur);
				}
				if (next_v == x)
				{
					bitmap |= 0x100;
					next_v = read_mem(++next);
				}

				if (bitmap_isalive(bitmap))
				{
					if (bitmap_fate(bitmap))
					{
						write_mem(++new, x - 1);
						new_v = x - 1;
					}
					else
						draw_dead(y, x);
				}
				else if (bitmap_fate(bitmap))
				{
					draw_alive(y, x);
					write_mem(++new, x - 1);
					new_v = x - 1;
				}
				else if (!bitmap)
					break;

				bitmap >>= 3;
				++x;

			} while (1);
		} while (1);
	} while (1);
	lcd_cursor(0, 150);
	lcd_printf("%04d", new - nextgen);
	next = thisgen;
	thisgen = nextgen;
	nextgen = next;
}


void main(void)
{
	init_system();

	while (1)
	{
		reset_simulation();
		choose_simulation();
		init_simulation();
		P4OUT ^= 0x40;
		do
		{
			step_simulation();
			P4OUT ^= 0x40;
		} while (!display_results(++generation));
	}
}


// END OF LINE.
