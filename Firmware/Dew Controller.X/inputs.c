#include "common.h"
#include "inputs.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
enum e_states {START = 0, CW1, CW2, CW3, CCW1, CCW2, CCW3};
enum e_flags {CW_FLAG = 0b10000000, CCW_FLAG = 0b01000000};

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// transition table for encoder FSM 
const uint8_t transition_table[7][4] = {
/*                 00           01      10      11  */
/* -----------------------------------------------------*/
/*START     | */  {START,       CCW1,   CW1,    START},
/*CW_Step1  | */  {CW2|CW_FLAG, START,  CW1,    START},
/*CW_Step2  | */  {CW2,         CW3,    CW1,    START},
/*CW_Step3  | */  {CW2,         CW3,    START,  START|CW_FLAG},
/*CCW_Step1 | */  {CCW2|CCW_FLAG,CCW1,  START,  START},
/*CCW_Step2 | */  {CCW2,        CCW1,   CCW3,   START},
/*CCW_Step3 | */  {CCW2,        START,  CCW3,   START|CCW_FLAG}
};

volatile uint8_t curRotState = START;
volatile enum e_direction rotDir = ROT_STOP;
volatile enum e_buttonPress pbState = PB_NONE;

//-----------------------------------------------------------------------------
// Rotary encoder ISR
// Sets rotDir to ROT_CW or ROT_CCW
//-----------------------------------------------------------------------------
void rotISR()
{
	uint8_t input;

	input = (ROT_B << 1) | ROT_A; // input code for rot_a and rot_b combined

	// set current state according to transition table (cw/ccw flags masked out)
	curRotState = transition_table[curRotState & 0b00000111][input];

	// set global direction flag
	if (curRotState & CW_FLAG) rotDir = ROT_CW;
	if (curRotState & CCW_FLAG) rotDir = ROT_CCW;
}

//-----------------------------------------------------------------------------
// Push button ISR
// Sets pbState to PB_SHORT, PB_LONG or PB_ABORT (too long :-))
//-----------------------------------------------------------------------------
void pushButtonISR()
{
	uint8_t time;
	// reset millisecond tick counter upon first rising edge of push button 
	if (!ROT_PB) {
		reset10msTick();
	} else {
		time = get10msTick();
		if ((time > 5) & (time <= 50)) 
			// short button press
			pbState = PB_SHORT;		
		else if ((time > 50) & (time <= 150))
			// long button press
			pbState = PB_LONG; 
		else 
			// button pressed too long => abort
			pbState = PB_ABORT;
	}
}

//-----------------------------------------------------------------------------
// Returns last rotary encoder action
//-----------------------------------------------------------------------------
enum e_direction getRotDir(void)
{
	enum e_direction ret = rotDir;
	rotDir = ROT_STOP;
	return ret;
}

//-----------------------------------------------------------------------------
// Returns last push button action
//-----------------------------------------------------------------------------
enum e_buttonPress getPB(void)
{
	enum e_buttonPress ret = pbState;
	pbState = PB_NONE;
	return ret;
}

//-----------------------------------------------------------------------------
// Simple up/down counter using the rotary encoder
//-----------------------------------------------------------------------------
void spinInput(float *input, float min, float max, float step)
{
	enum e_direction dir = getRotDir();
	
	if ((dir == ROT_CCW) && (*input > min))
		*input -= step;
	else if ((dir == ROT_CW) && (*input < max))
		*input += step;
}