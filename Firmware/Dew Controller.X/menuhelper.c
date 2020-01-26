#include "common.h"
#include "menu.h"
#include "menuhelper.h"
#include "oled.h"
#include "inputs.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define COLUMNS 12
#define ANY_PAGE 255
#define SCREEN_BUFFER_SIZE 49

enum e_menuStates {
	ST_STATUS_VIEW,
	ST_CHANNEL_VIEW,
	ST_CHANNEL_SETUP,
	ST_SET_OUTPUT_POWER,
	ST_SET_LENS_DIA
};

typedef uint8_t (*t_stateFuncPtr)(t_globalData*);

typedef struct {
	uint8_t state;
	uint8_t page;
	uint8_t pbShort;
	uint8_t pbLong;
	uint8_t timeout;
} t_nextState;


typedef struct {
	enum e_menuStates state;
	t_stateFuncPtr func;
} t_stateFunc;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
t_stateFuncPtr getStateFunc(enum e_menuStates state);
int8_t getNextState(enum e_menuStates state, uint8_t page, enum e_buttonPress pb);
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
	{ST_SETUP,			setup},
	{ST_SET_DP_OFFSET,	setDPOffset},
	{ST_SET_SKY_TEMP,	setSkyTemp},
	{ST_SET_FUDGE_FACTOR,	setFudgeFactor}
};

static const t_nextState nextStateTbl[] = {
	{ST_STATUS_VIEW,	ANY_PAGE,	ST_CHANNEL_VIEW,	ST_SETUP,		ST_STATUS_VIEW},
	{ST_CHANNEL_VIEW,	ANY_PAGE,	ST_STATUS_VIEW,		ST_CHANNEL_SETUP,	ST_CHANNEL_VIEW},
	{ST_CHANNEL_SETUP,	0,		ST_SET_OUTPUT_POWER,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	{ST_CHANNEL_SETUP,	1,		ST_SET_LENS_DIA,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	{ST_CHANNEL_SETUP,	2,		ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	{ST_SET_OUTPUT_POWER,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP},
	{ST_SET_LENS_DIA,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP},
	{ST_SETUP,			0,			ST_SET_DP_OFFSET,	ST_STATUS_VIEW, ST_STATUS_VIEW},
	{ST_SETUP,			1,			ST_SET_SKY_TEMP,	ST_STATUS_VIEW, ST_STATUS_VIEW},
	{ST_SETUP,			2,			ST_SET_FUDGE_FACTOR,	ST_STATUS_VIEW, ST_STATUS_VIEW},
	{ST_SET_DP_OFFSET,	0,			ST_SETUP,			ST_SETUP,		ST_SETUP},
	{ST_SET_SKY_TEMP,	0,			ST_SETUP,			ST_SETUP,		ST_SETUP},
	{ST_SET_FUDGE_FACTOR,	0,			ST_SETUP,			ST_SETUP,		ST_SETUP}
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
		menuError();
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
int8_t getNextState(enum e_menuStates state, uint8_t page, enum e_buttonPress pb)
{
	uint8_t n;
	
	// search for state inside nextStateTbl
	for(n = 0; n < len(nextStateTbl); n++) {
		if ((nextStateTbl[n].state == state)) {
			// compare page
			if ((nextStateTbl[n].page == page) || 
				(nextStateTbl[n].page == ANY_PAGE)) {
				// compare key press event
				if (pb == PB_SHORT)
					return nextStateTbl[n].pbShort;
				else if (pb == PB_LONG)
					return nextStateTbl[n].pbLong;
			}
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Status screen: Temperature, humidity, dew point and battery voltage
//-----------------------------------------------------------------------------
uint8_t statusView(t_globalData *data)
{
	static uint8_t page = 0;
	
	returnToPage(page);
	if (data->status.SENSOR_OK) {
		OLED_print_xy(0, 0, "Temperature Rel.humidityDewpoint    Bat.   Power");
		if (data->status.AUX_SENSOR_OK)
			sprintf(sBuf, "%3.1f\001\002%3.1f\001", data->tempC, data->tempAux);
		else
			sprintf(sBuf, "%5.1f\003      ", data->tempC);
		sprintf(lBuf, "%s%5.1f %%     %5.1f\001      %4.1fV  %4.1fW", 
			sBuf, data->relHum, data->dewPointC, data->voltage, data->power);
		OLED_print_xy(0, 1, lBuf);
		page = paging(page, 4);
	} else {
		page = 0;
		OLED_returnHome();
		OLED_print_xy(0, 0, "Bat.   Power");
		sprintf(lBuf, "%4.1fV  %4.1fW", data->voltage, data->power);
		OLED_print_xy(0, 1, lBuf);
	}
	return page;
}

//-----------------------------------------------------------------------------
// Channel view: Lens diameter and output power / channel status
//-----------------------------------------------------------------------------
uint8_t channelView(t_globalData *data)
{
	static uint8_t page = 0;
	uint8_t n;
	float tmp0;
	
	returnToPage(page);
	if (g_updateScreen) { 
		lBuf[0] = '\0';
		for(n = 0; n < NUM_CHANNELS; n++) {
			sprintf(sBuf, "Ch %1d: %2d\"   ", n + 1, data->chData[n].lensDia);
			strcat(lBuf,sBuf);
		}
		OLED_print_xy(0, 0, lBuf);
	}
	
	lBuf[0] = '\0';
	for(n = 0; n < NUM_CHANNELS; n++) {
		switch(data->chData[n].status) {
		case OFF:
			strcpy(sBuf, "Off         ");
			break;
		case ON:
			tmp0 = data->chData[n].Patt;
			sprintf(sBuf, "%4.1fW %s", tmp0, (data->chData[n].mode == AUTO ? "auto  " : "manual"));
			break;
		case OPEN:
			strcpy(sBuf, "Disconnected");
			break;
		case SHORTED:
			strcpy(sBuf, "Shorted!    ");
			break;
		case OVERCURRENT:
			strcpy(sBuf, "Overcurrent!");
			break;
		default:
			strcpy(sBuf, "OndreSpecial");
			break;
		}
		strcat(lBuf, sBuf);
	}
	OLED_print_xy(0, 1, lBuf);

	page = paging(page, 4);
	g_selectedChannel = page;
	
	return page;
}

//-----------------------------------------------------------------------------
// Channel setup menu: Output power (manual/auto) and lens diameter
//-----------------------------------------------------------------------------
uint8_t channelSetup(t_globalData *data)
{
	static uint8_t page = 0;
	
	returnToPage(page);
	OLED_print_xy(0, 0, "Output powerLens diam.  Back        ");
	sprintf(lBuf, "%1d           %1d           \003     ", g_selectedChannel, g_selectedChannel);
	OLED_print_xy(0,1,lBuf);
	page = paging(page, 3);
	return page;
}

//-----------------------------------------------------------------------------
// Set output power level
//-----------------------------------------------------------------------------
uint8_t setOutputPower(t_globalData *data)

	static float test;
	
	returnToPage(0);
	OLED_print_xy(0, 0, "Set outp    ");
	
	spinInput(&test, -5, 5, 0.25);
	sprintf(sBuf, "%5.2f       ", test);
	OLED_print_xy(0,1,sBuf);		
		
	return 0;
}

//-----------------------------------------------------------------------------
// Set lens diameter
//-----------------------------------------------------------------------------
uint8_t setLensDia(t_globalData *data)
{
	static float test;
	int8_t i;
	returnToPage(0);
	OLED_print_xy(0, 0, "Set lens   ");
	spinInput(&test, -5, 5, 1);
	i = test;
	sprintf(sBuf, "%2d          ", i);
	OLED_print_xy(0,1,sBuf);

	return 0;
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
	OLED_command(OLED_RETURNHOME);
	for(n = 0; n < COLUMNS * page; n++) {
		OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVELEFT);
	}
}
