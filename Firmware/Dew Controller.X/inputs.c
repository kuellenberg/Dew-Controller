#include "common.h"
#include "io.h"
#include "inputs.h"
#include "interrupt.h"

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

//-----------------------------------------------------------------------------
// Rotary encoder ISR
// Sets rotDir to ROT_CW or ROT_CCW
//-----------------------------------------------------------------------------
void rotISR()
{
	//uint8_t input;

	//input = (ROT_B << 1) | ROT_A; // input code for rot_a and rot_b combined

	// set current state according to transition table (cw/ccw flags masked out)
	curRotState = transition_table[curRotState & 0b00000111][(ROT_B << 1) | ROT_A];

	// set global direction flag
	if (curRotState & CW_FLAG) rotDir = ROT_CW;
	if (curRotState & CCW_FLAG) rotDir = ROT_CCW;
	
	userActivity = timeNow();
}

//-----------------------------------------------------------------------------
// Push button ISR
// Sets pbState to PB_SHORT, PB_LONG or PB_ABORT (too long :-))
//-----------------------------------------------------------------------------
void pushButtonISR()
{
	// reset millisecond tick counter upon first rising edge of push button 
	if ((!ROT_PB) && (pbState != PB_WAIT)) {
		tick10ms = 0;
		pbState = PB_WAIT;
		userActivity = timeNow();
	} else if (pbState == PB_WAIT) {
		if ((tick10ms > 10) && (tick10ms <= 70) && (ROT_PB)) 
			// short button press
			pbState = PB_SHORT;		
		else if ((tick10ms > 70) && (!ROT_PB))
			// long button press
			pbState = PB_LONG; 
		else if (ROT_PB)
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
	if (pbState != PB_WAIT)
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

void spinInputI(int8_t *input, int8_t min, int8_t max, int8_t step)
{
	enum e_direction dir = getRotDir();
	
	if ((dir == ROT_CCW) && (*input > min))
		*input -= step;
	else if ((dir == ROT_CW) && (*input < max))
		*input += step;
}