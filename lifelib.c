//	lifelib.c	03/24/2014
//******************************************************************************
//******************************************************************************
//
//	LL     IIII   FFFFFF  EEEEEE  LL     IIII  BBBBB        CCCCC
//	LL      II    FF      EE      LL      II   BB  BB      CC   CC
//	LL      II    FF      EE      LL      II   BB  BB      CC
//	LL      II    FFFF    EEEE    LL      II   BBBBB       CC
//	LL      II    FF      EE      LL      II   BB  BB      CC
//	LL      II    FF      EE      LL      II   BB  BB  ..  CC   CC
//	LLLLL  IIII   FF      EEEEEE  LLLLL  IIII  BBBBB   ..   CCCCC
//
//	!!!!!  DO NOT ALTER display_results function !!!!!
//
//******************************************************************************
//******************************************************************************

#include "lifelib.h"


volatile uint16 WDT_Sec_Cnt;			// WDT second counter
volatile uint16 WDT_debounce_cnt;		// WDT debounce counter
volatile uint16 seconds;				// # of seconds
volatile uint16 switches;				// debounced switch values


//	vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DO NOT ALTER vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//******************************************************************************
//	Call display_results after every generation
//
//	returns TRUE when MAX_TIME is reached or switch is pressed
//
int display_results(uint16 generation)
{
	static uint16 old_seconds;

	lcd_mode(0);
	lcd_cursor(4, 1);
	lcd_printf("%d", generation);

	// only update when seconds changes
	if (!seconds) old_seconds = 0;
	if (seconds != old_seconds)
	{
		old_seconds = seconds;
		printf("/%d", seconds);
		lcd_cursor(145, 1);
		printf("%2d", generation / seconds);
	}

	// check for max time exceeded
	if (seconds >= MAX_TIME)
	{
		lcd_cursor(50, 80);
		lcd_printf("\a\b\t%d.%02d", generation / MAX_TIME,
							generation % MAX_TIME);
		if (generation < MAX_TIME * 5)
		{
			lcd_cursor(30, 30);
			lcd_printf("\a\b\tTOO SLOW!");
		}
		switches = 0;				// clear any pending switches
		while (!switches);			// wait for any switch
	}
	return switches;				// return switch value
}
//******************************************************************************
//	^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DO NOT ALTER ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//------------------------------------------------------------------------------
//	Initialize watchdog --------------------------------------------------------
void watchdog_init()
{
	// configure Watchdog and Port 1
	WDTCTL = WDT_CTL;					// Set Watchdog interval
	WDT_Sec_Cnt = WDT_1SEC_CNT;			// set WD 1 second counter
	WDT_debounce_cnt = 0;				// clear debounce count
	IE1 |= WDTIE;						// enable WDT interrupt
	return;
} // end watchdog_init


void port1_init(void)
{
	// configure P1 switches and ADXL345 INT1 for interrupt
	P1SEL &= ~0x0f;						// select GPIO
	P1DIR &= ~0x0f;						// Configure P1.0-3 as Inputs
	P1OUT |= 0x0f;						// use pull-ups
	P1IES |= 0x0f;						// high to low transition
	P1REN |= 0x0f;						// Enable pull-ups
	P1IE |= 0x0f;						// P1.0-3 interrupt enabled
	P1IFG &= ~0x0f;						// P1.0-3 IFG cleared
	WDT_debounce_cnt = 0;				// clear debounce count
	switches = 0;						// clear switches flag
	return;
} // end port1_init


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Port 1 interrupt service routine
//
#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR(void)
{
	P1IFG &= ~0x0f;						// P1.0-3 IFG cleared
	WDT_debounce_cnt = DEBOUNCE_CNT;	// enable debounce
	return;
} // end Port_1_ISR


//------------------------------------------------------------------------------
// Watchdog Timer ISR ----------------------------------------------------------
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
{
	if (--WDT_Sec_Cnt == 0)
	{
		WDT_Sec_Cnt = WDT_1SEC_CNT;		// reset counter
		++seconds;						// count seconds
		LED_GREEN_TOGGLE;				// toggle green LED
	}

	// check for switch debounce
	if (WDT_debounce_cnt && (--WDT_debounce_cnt == 0))
	{
		switches = (P1IN ^ 0x0f) & 0x0f;
	}
} // end WDT_ISR(void)


//------------------------------------------------------------------------------
// un-initialized MSP430F2274 interrupt vectors
//
//    .int00       : {}               > INT00
//    .int01       : {}               > INT01
//    PORT1        : { * ( .int02 ) } > INT02 type = VECT_INIT
//    PORT2        : { * ( .int03 ) } > INT03 type = VECT_INIT
//    .int04       : {}               > INT04
//    ADC10        : { * ( .int05 ) } > INT05 type = VECT_INIT
//    USCIAB0TX    : { * ( .int06 ) } > INT06 type = VECT_INIT
//    USCIAB0RX    : { * ( .int07 ) } > INT07 type = VECT_INIT
//    TIMERA1      : { * ( .int08 ) } > INT08 type = VECT_INIT
//    TIMERA0      : { * ( .int09 ) } > INT09 type = VECT_INIT
//    WDT          : { * ( .int10 ) } > INT10 type = VECT_INIT
//    .int11       : {}               > INT11
//    TIMERB1      : { * ( .int12 ) } > INT12 type = VECT_INIT
//    TIMERB0      : { * ( .int13 ) } > INT13 type = VECT_INIT
//    NMI          : { * ( .int14 ) } > INT14 type = VECT_INIT
//    .reset       : {}               > RESET


// still lifes
const uint8 block[] = { "x = 2, y = 2, rule = B3/S23\n2o$2o!" };
const uint8 beehive[] = { "x = 4, y = 3, rule = B3/S23\nb2o$o2bo$b2o!" };
const uint8 loaf[] = { "x = 4, y = 4, rule = B3/S23\nb2o$o2bo$bobo$2bo!" };
const uint8 boat[] = { "x = 3, y = 3, rule = B3/S23\n2o$obo$bo!" };

// oscillators
const uint8 blinker[] = { "x = 3, y = 1, rule = B3/S23\n3o!" };
// const uint8 toad[] = { "x = 4, y = 2, rule = B3/S23\nb3o$3o!" };
const uint8 toad[] = { "x = 4, y = 2, rule = B3/S23\n3o$b3o!" };
const uint8 beacon[] = { "x = 4, y = 4, rule = B3/S23\n2o$o$3bo$2b2o!" };

const uint8 by_flop[] = { "x = 6, y = 7, rule = B3/S23\n"
		"3bo$bobo$5bo$5o$5bo$bobo$3bo!" };

const uint8 hexapole[] = {	"x = 9, y = 9, rule = B3/S23\n"
		"2o$obo2$2bobo2$4bobo2$6bobo$7b2o!"  };

const uint8 pulsar[] = { "x = 15, y = 15, rule = B3/S23\n"
		"4bo5bo$4bo5bo$4b2o3b2o2$3o2b2ob2o2b3o$2bobobobobobo$4b2o3b2o2$"
		"4b2o3b2o$2bobobobobobo$3o2b2ob2o2b3o2$4b2o3b2o$4bo5bo$4bo5bo!" };

// guns
const uint8 gosper_glider_gun[] = {	"x = 36, y = 9, rule = B3/S23\n"
		"24bo$22bobo$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o$"
		"2o8bo3bob2o4bobo$10bo5bo7bo$11bo3bo$12b2o!" };

const uint8 gosper_glider_gun2[] = { "x = 36, y = 9, rule = B3/S23\n"
		"12b2o$11bo3bo$10bo5bo7bo$2o8bo3bob2o4bobo$2o8bo5bo3b2o$"
		"11bo3bo4b2o12b2o$12b2o6b2o12b2o$22bobo$24bo!" };

// spaceships
const uint8 glider[] = { "x = 3, y = 3, rule = B3/S23\n3o$2bo$bo!" };
const uint8 glider1[] = { "x = 3, y = 3, rule = B3/S23\nbo$2bo$3o!" };

const uint8 lwss[] = { "x = 5, y = 4, rule = B3/S23\no2bo$4bo$o3bo$b4o!" };
const uint8 mwss[] = { "x = 6, y = 5, rule = B3/S23\n2bo$o3bo$5bo$o4bo$b5o!" };
const uint8 hwss[] = { "x = 7, y = 5, rule = B3/S23\n2b2o$o4bo$6bo$o5bo$b6o!" };

const uint8 bird[] = { "x = 10, y = 20, rule = B3/S23\n"
		"4bobo$3bo$3bo$3bo2bo$3b3o5$4b2o$2b2ob2o$2b4o$3b2o3$2b3o$bo2bo$4bo$4bo$bobo!"  };

const uint8 loafer[] = { "x = 9, y = 9, rule = B3/S23\n"
		"b2o2bob2o$o2bo2b2o$bobo$2bo$8bo$6b3o$5bo$6bo$7b2o!"
};

const uint8 canada_goose[] = { "x = 13, y = 12, rule = B3/S23\n"
		"10b3o$b2o9bo$ob3o6bo$4b2o2b2o$8bo$4bo$3bo3b2o$4b2obobo$b2obo2bobo$4b2o"
		"4bo$9b2o$9b2o!" };

// factories
const uint8 beacon_maker[] = { "x = 55, y = 54, rule = B3/S23\n"
		"53b2o$52bobo$51bo$50bo$49bo$"
		"48bo$47bo$46bo$45bo$44bo$43bo$42bo$41bo$40bo$39bo$38bo$37bo$36bo$35bo$"
		"34bo$33bo$32bo$31bo$30bo$29bo$28bo$27bo$26bo$25bo$24bo$23bo$22bo$21bo$"
		"20bo$19bo$18bo$17bo$16bo$15bo$14bo$13bo$12bo$11bo$10bo$9bo$8bo$7bo$6bo"
		"$5bo$4bo$3bo$3o$2bo$2bo!" };

const uint8 blinker_puffer[] = { "x = 9, y = 18, rule = B3/S23\n"
		"5bo$3bo3bo$8bo$3bo4bo$4b5o4$6b2o$3b3ob2o$4b4o$5b2o2$2b2o$"
		"o4bo$6bo$o5bo$b6o!" };

const uint8 carnivore[] = { "x = 31, y = 21, rule = B3/S23\n"
		"28b2o$28b2o2$26b4o$26bo3bo$29bo$29bo$2bo17bo7bo$o3bo13bo3bo4bobo$5bo4b"
		"5o8bo4bo$o4bo3bo4bo3bo4bo$b5o8bo4b5o4bo$9bo3bo13bobo$11bo16bo$29bo$29b"
		"o$26bo3bo$26b4o2$28b2o$28b2o!" };

const uint8 converter[] = { "x = 29, y = 32, rule = B3/S23\n"
		"6bo14b3o$7b2o11bo2bo$6b2o15bo$23bo$20bobo8$17bobo$18b2o$18bo5$15bo$14b"
		"2o8b2o$2o11b3obo5bo2bo$2o10b2o8b2ob3o$13b2o8b2obo$14bo10bo2$14bo$13b2o"
		"$2o10b2o13b2o$2o11b3obo9b2o$14b2o$15bo!" };

// HighLife patterns
const uint8 replicator[] = { "x = 4, y = 4, rule = B36/S23\nb3o$o3b$o3b$o!" };

//	#C Square Root of Minus Garfield -- "Conway's Game of Death"
//	#C Submission by Henning Makholm <henning@makholm.net>
//	#C
//	#C Pseudo-still life in the likeness of Jon Arbuckle found by
//	#C a randomized search, partially inspired by Paul Callahan's
//	#C online still life generator. Several thousand candidates
//	#C were produced and inspected to find one that would burn
//	#C quickly when hit by a glider.
const uint8 jon_arbuckle[] = { "x = 126, y = 75, rule = B3/S23\n"
		"51bo$"
		"37b2ob2o8bobo$"
		"36bo2b2o2bo3b2o2bo$"
		"33b2o2bo4b2o3bob2o$"
		"32bo2b2o7b2obo5bo$"
		"25bo2b2obob2o9bobob8o$"
		"23b3o3bobo2bo11bobo7bo$"
		"22bo3b3o2b3o12b2o3b4obo2bo$"
		"22bo2bo2bobo19bo3bob4o$"
		"20b2o4bobob2o18b2o$"
		"20bo6bo30b2o$"
		"17b2obo37bobo$"
		"16bobob2o30b2o6bo$"
		"13bo3bo2bo14bo15bobo5bo$"
		"13b4o3bo13bobo8bobo3bobob2obo$"
		"17b3o15bobo3b2obob4obob2obob2o$"
		"15bobo18b2o2bobobo5bo$"
		"14bobo23b2o3b5o$"
		"14b2o20b2o14b2ob2obo$"
		"12b2o21bobo7b5o2bob2ob2o$"
		"13bo21bo7b3o2bobobo6b2o$"
		"12bo23bo5bo8b2o6bobo$"
		"12b2o19b4o4bobo9b2o5b2o$"
		"13bo15b2o2bo6bobo10bobo6b2o$"
		"12bo16bo4bo5b2o13bo6bo$"
		"13b3o14b4o20bo8bo$"
		"16bo23b2o12b4o6bo$"
		"13b3obob2o9b2o8b2o15bo5b2o$"
		"13bo2b2obobo8b2o24bo6bo$"
		"14bo4bobo18b2o14b2o6bo$"
		"13b2ob4obob2o5b2o9bo21b2o$"
		"12bo3bo4bo2bo4bobo8bo15b2o5bo$"
		"12b3o2b3ob2o6bo10b2o14bo7bo$"
		"15b2o2bobo5bob2o10bo15bo5b2o$"
		"12b2o3bobobobo2bobo11bo15b2o$"
		"12bob4obobob4obo11b2o14bo6b2o$"
		"13bo4bobobo5b2o11bo11b2obo2b2obobo$"
		"14b3obobo3b4o12bo5b2ob2obobobobobobo$"
		"16bobob4o3bo13b3obob2obobobobobobob2o$"
		"16bobo5bo18bobo5b2obobobo2bo$"
		"15b2obob4o20b2o8b2obobobo$"
		"16bobobo25b2o3b3o3bobob2o$"
		"16bobo2bo24bobobo2bo3b2o$"
		"15b2ob4o3b2obo2bo17b2o13b2o$"
		"17bo6bobob4o23b4o5bo$"
		"17bob3o2bobo5b2o21bo2bo6bo$"
		"16b2obo2bobo2bob2obobo21b2o8bo$"
		"17bobobobo2b2obobo2bo30b2o$"
		"17bobobo6bo5b2o20b2o6bo$"
		"16b2obob2o3b2o28bo8b2o$"
		"17bobobo4bo7b2o21bo9bo$"
		"17bobobo2bobo7bo23bo6b2o$"
		"16b2ob2o3b2o9bo21b2o6bo$"
		"15bo2bo13b4o21bo8bo$"
		"15b2obo5b2o6bo21b2obo6bobo$"
		"16bob2o3bobo7bo14b2ob2o2bob2o5b2o$"
		"15bo3bo3bo8b2o13bobobob2o$"
		"15bob2o5bo6bo15bo2bo13b2o$"
		"14b2obo5b2o6bo13b2ob2o14bo$"
		"15bobo11b2ob2o12bobo13bobo$"
		"15bob2o11bobobob2o8bobo4bo8b2o$"
		"14b2o14bobobobo2b2o4b2ob2o2bobo5b2o$"
		"15bo15bo3bo2b2o2bo9b2o5bobo$"
		"15bobo23b2o16b2o$16b2o25b2o12b2o$"
		"18b2o23bobo5b2obobobo$"
		"18bo2bo2bo19bobob2o2bob2o$"
		"19b6o20b2obob2o$"
		"25b2o21bo$"
		"21b2obo2bob2o14b4o$"
		"20bobob4obobo13bo3b2o$"
		"20b2o7bobo2bo8b2o2b2o2bo$"
		"18b2o6b2obob4o7bo2b2obob2o$"
		"19bo7bob2o9bo2b2o3bobo$"
		"17bo9bo4b3o3b5o2b4o2bo"
		"!"
		} ;

