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
			row += repeat;
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

void cell_spawn(uint8 row, uint8 col)
{
	universe[row][col >> COLSPOW] |= (0x80 >> (col & COLSPOWSEL));
	draw_alive(row, col);
}

void cell_kill(uint8 row, uint8 col)
{
	universe[row][col >> COLSPOW] &= ~(0x80 >> (col & COLSPOWSEL));
	draw_dead(row, col);
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
	seconds = 0;
	switches = 0;
	generation = 0;
	memset(universe, 0, sizeof(universe));
	memset(workingrows, 0, sizeof(workingrows));
	lcd_clear();

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
		draw_rle_pattern(20, 20, gosper_glider_gun2);

		do
		{
			step_simulation();
			P4OUT ^= 0x40;
		} while (!display_results(++generation));


	}
}


// END OF LINE.
