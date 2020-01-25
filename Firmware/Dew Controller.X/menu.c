#include "common.h"
#include "menu.h"
#include "oled.h"
#include "inputs.h"

#define COLUMNS 12
#define ANY_PAGE 255
#define SCREEN_BUFFER_SIZE 49

static char sBuf1[SCREEN_BUFFER_SIZE], sBuf2[COLUMNS];

uint8_t statusView(t_globalData *data);
uint8_t channelView(t_globalData *data);
uint8_t channelSetup(t_globalData *data);
uint8_t setOutputPower(t_globalData *data);
uint8_t setLensDia(t_globalData *data);
uint8_t paging(uint8_t currentPage, const uint8_t lastPage);
void getStateFunc(enum e_menuStates state);
int8_t getNextState(enum e_menuStates state, uint8_t page, enum e_buttonPress pb);
static void returnToPage(uint8_t page);
void menuError();

enum e_menuStates {
	ST_STATUS_VIEW,
	ST_CHANNEL_VIEW,
	ST_CHANNEL_SETUP,
	ST_SET_OUTPUT_POWER,
	ST_SET_LENS_DIA
};

static uint8_t (*p_fct[])(t_globalData*) = {
	statusView,
	channelView,
	channelSetup,
	setOutputPower,
	setLensDia
};

typedef struct {
	uint8_t state;
	uint8_t page;
	uint8_t pbShort;
	uint8_t pbLong;
	uint8_t timeout;
} t_nextState;

typedef uint8_t (*t_stateFunc)(t_globalData*);

typedef struct {
	enum e_menuStates state;
	t_stateFunc func;
} t_stateFunc;

static const t_stateFunc stateFuncTbl[] = {
	{ST_STATUS_VIEW, statusView},
	{ST_CHANNEL_VIEW, channelView},
	{ST_CHANNEL_SETUP, channelSetup},
	{ST_SET_OUTPUT_POWER, setOutputPower},
	{ST_SET_LENS_DIA, setLensDia}
};

static const t_nextState nextStateTbl[] = {
	{ST_STATUS_VIEW,	ANY_PAGE,	ST_CHANNEL_VIEW,	ST_STATUS_VIEW,		ST_STATUS_VIEW},
	{ST_CHANNEL_VIEW,	ANY_PAGE,	ST_STATUS_VIEW,		ST_CHANNEL_SETUP,	ST_CHANNEL_VIEW},
	{ST_CHANNEL_SETUP,	0,		ST_SET_OUTPUT_POWER,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	{ST_CHANNEL_SETUP,	1,		ST_SET_LENS_DIA,	ST_CHANNEL_VIEW,	ST_CHANNEL_VIEW},
	{ST_SET_OUTPUT_POWER,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP},
	{ST_SET_LENS_DIA,	0,		ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP,	ST_CHANNEL_SETUP}
};

static uint8_t updateScreen = 1;
static uint8_t selectedChannel = 0;

//-----------------------------------------------------------------------------
// Main menu screen
//-----------------------------------------------------------------------------
void menu(t_globalData *data)
{
	static uint8_t state = 0;
	int8_t page, nextState;
	enum e_buttonPress pb;
	t_stateFunc func;
	
	//page = (p_fct[state])(data);
	func = getStateFunc(state);
	if (func)
		page = (*func)(data);
	else 
		menuError(); // replace with error handling code
	updateScreen = 0;
	pb = getPB();
	nextState = getNextState(state, page, pb);	
	if (nextState > -1) {
		if (state != nextState) {
			updateScreen = 1;
			state = nextState;
		}
	}
}

void menuError()
{
	OLED_returnHome();
	OLED_clearDisplay();
	OLED_print_xy(0, 0, "-> Fuckup <-");
	while(1);
}

t_stateFunc getStateFunc(enum e_menuStates state)
{
	uint8_t n;
	
	for(n = 0; n < sizeof(stateFuncTbl)/sizeof(t_stateFunc); n++) {
		if ((stateFuncTbl[n].state == state))
			return stateFuncTbl[n].func;
	}
	return (t_stateFunc)NULL;
}


int8_t getNextState(enum e_menuStates state, uint8_t page, enum e_buttonPress pb)
{
	uint8_t n;
	
	for(n = 0; n < sizeof(nextStateTbl)/sizeof(t_nextState); n++) {
		if ((nextStateTbl[n].state == state)) {
			if ((nextStateTbl[n].page == page) || 
				(nextStateTbl[n].page == ANY_PAGE)) {
				if (pb == PB_SHORT)
					return nextStateTbl[n].pbShort;
				else if (pb == PB_LONG)
					return nextStateTbl[n].pbLong;
			}
		}
	}
	return -1;
}

uint8_t statusView(t_globalData *data)
{
	static uint8_t page = 0;
	
	returnToPage(page);
	if (data->status.SENSOR_OK) {
		OLED_print_xy(0, 0, "Temperature Rel.humidityDewpoint    Bat.   Power");
		if (data->status.AUX_SENSOR_OK)
			sprintf(sBuf2, "%3.0f | %3.0f \337C", data->tempC, data->tempAux);
		else
			sprintf(sBuf2, "%5.1f \337C    ", data->tempC);
		sprintf(sBuf1, "%s%5.1f %%     %5.1f \337C    %4.1fV  %4.1fW", 
			sBuf2, data->relHum, data->dewPointC, data->voltage, data->power);
		OLED_print_xy(0, 1, sBuf1);
		page = paging(page, 4);
	} else {
		page = 0;
		OLED_returnHome();
		OLED_print_xy(0, 0, "Bat.   Power");
		sprintf(sBuf1, "%4.1fV  %4.1fW", data->voltage, data->power);
		OLED_print_xy(0, 1, sBuf1);
	}
	return page;
}

uint8_t channelView(t_globalData *data)
{
	static uint8_t page = 0;
	
	returnToPage(page);
	if (updateScreen) 

	sBuf1 = '\0';
	for(n = 0; n < NUM_CHANNELS; n++) {
		sprintf(sBuf2, "Ch%2d: %2d inch", n, data->chData[n].lensDia);
		strcat(sBuf1,sBuf2);
	}
	OLED_print_xy(0, 0, s);
	sBuf1 = '\0';
	for(n = 0; n < NUM_CHANNELS; n++) {
		switch(data->chData[n].status) {
		case OFF:
			strcpy(sBuf2, "Off         ");
			break;
		case ON:
			sprintf(sBuf2, "%4.1fW %s", data->chData[n].Patt, (data->chData[n].mode == AUTO ? "auto   " : "manual"));
			break;
		case OPEN:
			strcpy(sBuf2, "Disconnected");
			breakl
		case SHORT:
			strcpy(sBuf2, "Shorted!");
			break;
		case OVERCURRENT:
			strcpy(sBuf2, "Overcurrent!");
			break;
		default:
			strcpy(sBuf2, "OndreSpecial");
			break;
		}
		strcat(sBuf1, sBuf2);
	}
	OLED_print_xy(0, 1, sBuf1);

	page = paging(page, 4);
	selectedChannel = page;
	return page;
}

uint8_t channelSetup(t_globalData *data)
{
	static uint8_t page = 0;
	
	returnToPage(page);
	OLED_print_xy(0, 0, "Output power Lens diam. ");
	sprintf(sBuf1, "%1d            %1d", selectedChannel, selectedChannel);
	OLED_print_xy(0,1,sBuf1);

	page = paging(page, 2);
	return page;
}

uint8_t setOutputPower(t_globalData *data)
{
	returnToPage(0);
	OLED_print_xy(0, 0, "Set outp    ");
	sprintf(sBuf2, "%1d          ", selectedChannel);
	OLED_print_xy(0,1,sBuf2);

	return 0;
}

uint8_t setLensDia(t_globalData *data)
{
	returnToPage(0);
	OLED_print_xy(0, 0, "Set lens   ");
	sprintf(sBuf2, "%1d          ", selectedChannel);
	OLED_print_xy(0,1,sBuf2);

	return 0;
}

uint8_t paging(uint8_t currentPage, const uint8_t lastPage)
{
	uint8_t n;
	enum e_direction dir;
	
	PIE0bits.IOCIE = 0;
	dir = getRotDir();
	
	if ((dir == ROT_CW) && (currentPage < lastPage - 1)) {
		currentPage++;
		for(n = 0; n < COLUMNS; n++) {
			OLED_scrollDisplayLeft();
			__delay_ms(20);
		}
	}
	else if ((dir == ROT_CCW) && (currentPage > 0)) {
		currentPage--;
		for(n = 0; n < COLUMNS; n++) {
			OLED_scrollDisplayRight();
			__delay_ms(20);
		}
	}
	PIE0bits.IOCIE = 1;
	return currentPage;	
}

void returnToPage(uint8_t page)
{
	uint8_t n;
	if (! updateScreen)
		return;
	OLED_command(OLED_RETURNHOME);
	for(n = 0; n < COLUMNS * page; n++) {
		OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVELEFT);
	}
}
