/*
 * conway.c
 *
 *  Created on: Nov 13, 2014
 *      Author: Kevin Haroldsen
 */


#include "conway.h"

COLSTYPE universe[ROWS][COLSECTS];
COLSTYPE workingrows[4][COLSECTS];
int generation;


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
	universe[row][col >> COLSPOW] |= (0x80 >> (col & COLSPOWSEL));
	draw_alive(row, col);
}

void cell_kill(uint8 row, uint8 col)
{
	universe[row][col >> COLSPOW] &= ~(0x80 >> (col & COLSPOWSEL));
	draw_dead(row, col);
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
	memset(universe, 0, sizeof(universe));
	memset(workingrows, 0, sizeof(workingrows));
	lcd_clear();
	lcd_rectangle(0, 0, 160, 160, 1);
	seconds = 0;
	generation = 0;
}

uint8 number_bits(uint8 number)
{
	register int n = 0;
	while (number)
	{
		number &= number - 1;
		++n;
	}
	return n;
}

void step_simulation()
{
	uint8 row, colg, coli;
	COLSTYPE *prev = workingrows[0],
			*cur = workingrows[1],
			*next = workingrows[2],
			*first = workingrows[3],
			*tmp;
	memcpy(cur, universe[ROWS-1], COLSECTS);
	memcpy(next, universe[0], COLSECTS);
	memcpy(first, universe[0], COLSECTS);
	for (row=0; row<ROWS; ++row)
	{
		tmp = prev;
		prev = cur;
		cur = next;
		next = tmp;
		if (row == ROWS - 1)
			next = first;
		else
			memcpy(next, universe[row + 1], COLSECTS);


		for (colg=0; colg<COLSECTS; ++colg)
		{
			uint8 nn, alive;


			// Cover the left edge
			alive = cur[colg] & COLSLEFT;
			nn = number_bits((prev[colg] & COLSLEFT2) |
							((next[colg] & COLSLEFT2) >> 2) |
							((cur[colg] & COLS2NDLEFT) >> 3));


			nn += number_bits((prev[(colg + COLSECTS - 1) % COLSECTS] & 0x01) |
							((cur[(colg + COLSECTS - 1) % COLSECTS] & 0x01) << 1) |
							((next[(colg + COLSECTS - 1) % COLSECTS] & 0x01) << 2));

			if (alive)
			{
				if (nn < 2 || nn > 3)
					cell_kill(row, colg << COLSPOW);
			}
			else if (nn == 3)
				cell_spawn(row, colg << COLSPOW);

			// Handle the middle cases
			for (coli=1; coli<(COLSSIZE-1); ++coli)
			{
				nn = 0;
				alive = cur[colg] & (COLSLEFT >> coli);
				nn = number_bits((cur[colg] & ((COLSLEFT >> 1 | COLSLEFT << 1) >> coli))) +
						number_bits(
								// (row & (0b11100000 >> (coli-1))) >> (coli-1)
						((prev[colg] & ((COLSLEFT2 | COLSLEFT >> 2) >> (coli - 1))) << (coli - 1)) |
						(((next[colg] & ((COLSLEFT2 | COLSLEFT >> 2) >> (coli - 1))) << (coli - 1)) >> 3)
						);

				if (alive)
				{
					if (nn < 2 || nn > 3)
					{
						cell_kill(row, coli + (colg << COLSPOW));
					}
				}
				else if (nn == 3)
					cell_spawn(row, coli + (colg << COLSPOW));
			}

			// Handle the right edge
			alive = cur[colg] & 0x01;
			nn = number_bits((prev[colg] & 3) |
							((cur[colg] & 2) << 1) |
							((next[colg] & 3) << 3) |
							(prev[(colg + 1) % COLSECTS] & COLSLEFT) |
							((cur[(colg + 1) % COLSECTS] & COLSLEFT) >> 1) |
							((next[(colg + 1) % COLSECTS] & COLSLEFT) >> 2));

			if (alive)
			{
				if (nn < 2 || nn > 3)
					cell_kill(row, COLSSIZE - 1 + (colg << COLSPOW));
			}
			else if (nn == 3)
				cell_spawn(row, COLSSIZE - 1 + (colg << COLSPOW));
		}
	}
}


void main(void)
{
	init_system();

	while (1)
	{
		reset_simulation();
		choose_simulation();

		do
		{
			step_simulation();
			P4OUT ^= 0x40;
		} while (!display_results(++generation));
	}
}


// END OF LINE.
