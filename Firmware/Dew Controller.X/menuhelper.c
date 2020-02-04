#include "common.h"
#include "menu.h"
#include "menuhelper.h"
#include "inputs.h"
#include "interrupt.h"
#include "error.h"
#include "oled.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define ST_ANY 255
#define MENU_TIMEOUT 1200
#define PAGING_DELAY_MS 30

enum e_menuStates {
	ST_STATUS_VIEW,
	ST_CHANNEL_VIEW,
	ST_CHANNEL_SETUP,
	ST_SET_OUTPUT_POWER,
	ST_SET_LENS_DIA,
	ST_SETUP,
	ST_SET_DP_OFFSET,
	ST_SET_SKY_TEMP,
	ST_SET_FUDGE_FACTOR,
	ST_SET_DEBUG_MODE,
	ST_NONE
};

typedef uint8_t (*t_stateFuncPtr)(void);

typedef struct {
	uint8_t state;
	uint8_t intState;
	uint8_t pbShort;
	uint8_t pbLong;
	uint8_t timeout;
	uint8_t noAction;
} t_nextState;


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
int8_t getNextState(enum e_menuStates state, uint8_t intState, 
			enum e_buttonPress pb, uint8_t timeout);

//-----------------------------------------------------------------------------
// Global variables 
//-----------------------------------------------------------------------------
static const t_stateFuncPtr stateFuncTbl[] = {
	statusView,
	channelView,
	channelSetup,
	setOutputPower,
	setLensDia,
	setup,
	setDPOffset,
	setSkyTemp,
	setFudgeFactor,
	setDebugMode
};

static const t_nextState nextStateTbl[] = {
	//			exit page or
	// current state	internal state	pb_short		pb long			timeout			no action
	{ST_STATUS_VIEW,	ST_ANY,		ST_CHANNEL_VIEW,	ST_SETUP,		ST_STATUS_VIEW,		ST_STATUS_VIEW},
	
	{ST_CHANNEL_VIEW,	ST_ANY,		ST_STATUS_VIEW,		ST_CHANNEL_SETUP,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	
	{ST_CHANNEL_SETUP,	0,		ST_SET_OUTPUT_POWER,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_SETUP},
	{ST_CHANNEL_SETUP,	1,		ST_SET_LENS_DIA,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_SETUP},
	{ST_CHANNEL_SETUP,	2,		ST_CHANNEL_SETUP,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_SETUP},
	
	{ST_SET_OUTPUT_POWER,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_SET_OUTPUT_POWER},
	{ST_SET_OUTPUT_POWER,	1,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP},
	
	{ST_SET_LENS_DIA,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP},
	{ST_SET_LENS_DIA,	1,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_SET_LENS_DIA},
	
	{ST_SETUP,		0,		ST_SET_DP_OFFSET,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	{ST_SETUP,		1,		ST_SET_SKY_TEMP,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	{ST_SETUP,		2,		ST_SET_FUDGE_FACTOR,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	{ST_SETUP,		3,		ST_SET_DEBUG_MODE,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	
	{ST_SET_DP_OFFSET,	0,		ST_SET_DP_OFFSET,	ST_SETUP,		ST_SETUP,		ST_SETUP},
	{ST_SET_DP_OFFSET,	1,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_DP_OFFSET},
	
	{ST_SET_SKY_TEMP,	0,		ST_SET_SKY_TEMP,	ST_SETUP,		ST_SETUP,		ST_SETUP},
	{ST_SET_SKY_TEMP,	1,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_SKY_TEMP},
	
	{ST_SET_FUDGE_FACTOR,	0,		ST_SET_FUDGE_FACTOR,	ST_SETUP,		ST_SETUP,		ST_SETUP},
	{ST_SET_FUDGE_FACTOR,	1,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_FUDGE_FACTOR},
	
	{ST_SET_DEBUG_MODE,	0,		ST_SET_DEBUG_MODE,	ST_SETUP,		ST_SETUP,		ST_SETUP},
	{ST_SET_DEBUG_MODE,	1,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_DEBUG_MODE}
};


//-----------------------------------------------------------------------------
// Menu function dispatcher
//-----------------------------------------------------------------------------
void menu(void)
{
	static uint8_t state = ST_STATUS_VIEW;
	int8_t page, nextState;
	//uint8_t timeout = 0;
	//enum e_buttonPress pb;
	//t_stateFuncPtr func;

	// call menu function according to current state
	page = (*stateFuncTbl[state])();
	
	g_screenRefresh = 0;
	// next state depends on current state, exit page and key press
	nextState = getNextState(state, page, getPB(), 
			(timeSince(userActivity) > MENU_TIMEOUT));
	
	if (nextState > -1) {
		// if state has changed, screen update is required
		if (state != nextState) {
			g_screenRefresh = 1;
			state = nextState;
		}
	} else {
		error(ERR_MENU);
	}
}


//-----------------------------------------------------------------------------
// Returns next state depending on current state, exit page and key press event
//-----------------------------------------------------------------------------
int8_t getNextState(enum e_menuStates state, uint8_t intState, 
			enum e_buttonPress pb, uint8_t timeout)
{
	uint8_t n;
	
	// search for state inside nextStateTbl
	for(n = 0; n < len(nextStateTbl); n++) {
		if ((nextStateTbl[n].state == state)) {
			// compare internal state ( = exit page)
			if ((nextStateTbl[n].intState == intState) || 
				(nextStateTbl[n].intState == ST_ANY)) {
				if (timeout)
					return nextStateTbl[n].timeout;
				// compare key press event
				if (pb == PB_SHORT)
					return nextStateTbl[n].pbShort;
				else if (pb == PB_LONG)
					return nextStateTbl[n].pbLong;
				else
					return nextStateTbl[n].noAction;
			}
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Handles rotary encoder action and display scrolling.
// Returns new page number
//-----------------------------------------------------------------------------
uint8_t paging(uint8_t currentPage, const uint8_t lastPage)
{
	uint8_t n;
	enum e_direction dir;
	//static enum e_direction lastDir = ROT_STOP;
	
	// disable interrupts during scrolling to prevent overshoot
	PIE0bits.IOCIE = 0;	
	
	dir = getRotDir();
	
	// two encoder steps required for screen paging
	//if (dir == lastDir) {
		// clockwise rotation of encoder
		if ((dir == ROT_CW) && (currentPage < lastPage - 1)) {
			currentPage++;
			for(n = 0; n < COLUMNS; n++) {
				OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVELEFT);
				__delay_ms(PAGING_DELAY_MS);
			}
		}
		// counter-clockwise rotation of encoder
		else if ((dir == ROT_CCW) && (currentPage > 0)) {
			currentPage--;
			for(n = 0; n < COLUMNS; n++) {
				OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVERIGHT);;
				__delay_ms(PAGING_DELAY_MS);
			}
		}
	//}
	
	//lastDir = dir;
	PIE0bits.IOCIE = 1; // enable interrupts
	
	return currentPage;	
}

//-----------------------------------------------------------------------------
// Sets display view port back to 'page'. 
// Needs to be called at the beginning of every menu function.
//-----------------------------------------------------------------------------
void returnToPage(uint8_t page)
{
	uint8_t n;
	
	if (! g_screenRefresh)
		return;
	OLED_command(OLED_CLEARDISPLAY);
	OLED_command(OLED_RETURNHOME);
	for(n = 0; n < COLUMNS * page; n++) {
		OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVELEFT);
	}
}
