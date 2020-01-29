#include "common.h"
#include "menu.h"
#include "menuhelper.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define ANY_PAGE 255

enum e_menuStates {
	ST_STATUS_VIEW,
	ST_CHANNEL_VIEW,
	ST_CHANNEL_SETUP,
	ST_SET_OUTPUT_POWER,
	ST_SET_LENS_DIA,
	ST_SETUP,
	ST_SET_DP_OFFSET,
	ST_SET_SKY_TEMP,
	ST_SET_FUDGE_FACTOR
};

typedef uint8_t (*t_stateFuncPtr)(t_globalData*);

typedef struct {
	uint8_t state;
	uint8_t intState;
	uint8_t pbShort;
	uint8_t pbLong;
	uint8_t timeout;
	uint8_t noAction;
} t_nextState;


typedef struct {
	enum e_menuStates state;
	t_stateFuncPtr func;
} t_stateFunc;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
t_stateFuncPtr getStateFunc(enum e_menuStates state);
int8_t getNextState(enum e_menuStates state, uint8_t intState, enum e_buttonPress pb);
void menuError(void);

//-----------------------------------------------------------------------------
// Global variables 
//-----------------------------------------------------------------------------
static uint8_t (*p_fct[])(t_globalData*) = {
	statusView,
	channelView,
	channelSetup,
	setOutputPower,
	setLensDia,
	setup,
	setDPOffset,
	setSkyTemp,
	setFudgeFactor
};

static const t_stateFunc stateFuncTbl[] = {
	{ST_STATUS_VIEW,	statusView},
	{ST_CHANNEL_VIEW,	channelView},
	{ST_CHANNEL_SETUP,	channelSetup},
	{ST_SET_OUTPUT_POWER,	setOutputPower},
	{ST_SET_LENS_DIA,	setLensDia},
	{ST_SETUP,		setup},
	{ST_SET_DP_OFFSET,	setDPOffset},
	{ST_SET_SKY_TEMP,	setSkyTemp},
	{ST_SET_FUDGE_FACTOR,	setFudgeFactor}
};

static const t_nextState nextStateTbl[] = {
	{ST_STATUS_VIEW,	ANY_PAGE,	ST_CHANNEL_VIEW,	ST_SETUP,		ST_STATUS_VIEW,		ST_STATUS_VIEW},
	{ST_CHANNEL_VIEW,	ANY_PAGE,	ST_STATUS_VIEW,		ST_CHANNEL_SETUP,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	{ST_CHANNEL_SETUP,	0,		ST_SET_OUTPUT_POWER,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_SETUP},
	{ST_CHANNEL_SETUP,	1,		ST_SET_LENS_DIA,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_SETUP},
	{ST_CHANNEL_SETUP,	2,		ST_CHANNEL_SETUP,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_SETUP}, // required?
	{ST_SET_OUTPUT_POWER,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_SET_OUTPUT_POWER},
	{ST_SET_OUTPUT_POWER,	1,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP},
	{ST_SET_LENS_DIA,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_SET_LENS_DIA},
	{ST_SETUP,		0,		ST_SET_DP_OFFSET,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	{ST_SETUP,		1,		ST_SET_SKY_TEMP,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	{ST_SETUP,		2,		ST_SET_FUDGE_FACTOR,	ST_STATUS_VIEW,		ST_STATUS_VIEW,		ST_SETUP},
	{ST_SET_DP_OFFSET,	0,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_DP_OFFSET},
	{ST_SET_SKY_TEMP,	0,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_SKY_TEMP},
	{ST_SET_FUDGE_FACTOR,	0,		ST_SETUP,		ST_SETUP,		ST_SETUP,		ST_SET_FUDGE_FACTOR}
};


//-----------------------------------------------------------------------------
// Menu function dispatcher
//-----------------------------------------------------------------------------
void menu(t_globalData *data)
{
	static uint8_t state = ST_STATUS_VIEW;
	int8_t page, nextState;
	enum e_buttonPress pb;
	t_stateFuncPtr func;
	
	// call menu function according to current state
	func = getStateFunc(state);
	if (func)
		page = (*func)(data);
	else 
		menuError(); // replace with error handling code
	
	g_updateScreen = 0;
	pb = getPB();
	// next state depends on current state, exit page and key press
	nextState = getNextState(state, page, pb);
	
	if (nextState > -1) {
		// if state has changed, screen update is required
		if (state != nextState) {
			g_updateScreen = 1;
			state = nextState;
		}
	} else {
		//menuError();
	}
}

//-----------------------------------------------------------------------------
// Menu error. This should never occur :-)
//-----------------------------------------------------------------------------
void menuError(void)
{
	OLED_returnHome();
	OLED_clearDisplay();
	OLED_print_xy(0, 0, "Menu error");
	while(1);
}

//-----------------------------------------------------------------------------
// Returns menu function according to current menu state
//-----------------------------------------------------------------------------
t_stateFuncPtr getStateFunc(enum e_menuStates state)
{
	uint8_t n;
	
	// search for state inside stateFuncTbl
	for(n = 0; n < len(stateFuncTbl); n++) {
		if ((stateFuncTbl[n].state == state))
			return stateFuncTbl[n].func;
	}
	return (t_stateFuncPtr)NULL;
}

//-----------------------------------------------------------------------------
// Returns next state depending on current state, exit page and key press event
//-----------------------------------------------------------------------------
int8_t getNextState(enum e_menuStates state, uint8_t intState, enum e_buttonPress pb)
{
	uint8_t n;
	
	// search for state inside nextStateTbl
	for(n = 0; n < len(nextStateTbl); n++) {
		if ((nextStateTbl[n].state == state)) {
			// compare page
			if ((nextStateTbl[n].intState == intState) || 
				(nextStateTbl[n].intState == ANY_PAGE)) {
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
	static enum e_direction lastDir = ROT_STOP;
	
	// disable interrupts during scrolling to prevent overshoot
	PIE0bits.IOCIE = 0;	
	
	dir = getRotDir();
	
	// two encoder steps required for screen paging
	if (dir == lastDir) {
		// clockwise rotation of encoder
		if ((dir == ROT_CW) && (currentPage < lastPage - 1)) {
			currentPage++;
			for(n = 0; n < COLUMNS; n++) {
				OLED_scrollDisplayLeft();
				__delay_ms(20);
			}
		}
		// counter-clockwise rotation of encoder
		else if ((dir == ROT_CCW) && (currentPage > 0)) {
			currentPage--;
			for(n = 0; n < COLUMNS; n++) {
				OLED_scrollDisplayRight();
				__delay_ms(20);
			}
		}
	}
	
	lastDir = dir;
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
	
	if (! g_updateScreen)
		return;
	OLED_command(OLED_CLEARDISPLAY);
	OLED_command(OLED_RETURNHOME);
	for(n = 0; n < COLUMNS * page; n++) {
		OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVELEFT);
	}
}
