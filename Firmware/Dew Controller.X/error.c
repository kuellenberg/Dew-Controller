#include "common.h"
#include "error.h"
#include "menuhelper.h"
#include "oled.h"
#include "inputs.h"
#include "io.h"

#define NUM_ERROR_MESSAGES 10

static enum e_errorcode errorMessageQueue[NUM_ERROR_MESSAGES] = {0};
static uint8_t head = 0;
static uint8_t tail = 0;

//-----------------------------------------------------------------------------
// add error to queue
//-----------------------------------------------------------------------------
void error(enum e_errorcode code)
{
	errorMessageQueue[head] = code;
	head = (head + 1) % NUM_ERROR_MESSAGES;
	if (head == tail)
		tail = (tail + 1) % NUM_ERROR_MESSAGES;
}

//-----------------------------------------------------------------------------
// return last error from queue
//-----------------------------------------------------------------------------
enum e_errorcode getLastError(void)
{	
	return (head == tail) ? NO_ERROR : errorMessageQueue[tail];
}

//-----------------------------------------------------------------------------
// view error messages
//-----------------------------------------------------------------------------
void viewErrorMessage(void)
{
	enum e_errorcode errorCode;
	
	errorCode = (head == tail) ? NO_ERROR : errorMessageQueue[tail];	
	
	OLED_command(OLED_RETURNHOME);
	switch(errorCode) {
	case WARN_REMOVED1:
		OLED_print_xy(0, 0, "Heater 1    ");
		OLED_print_xy(0, 1, "removed     ");
		break;
	case WARN_REMOVED2:
		OLED_print_xy(0, 0, "Heater 2    ");
		OLED_print_xy(0, 1, "removed     ");
		break;
	case WARN_REMOVED3:
		OLED_print_xy(0, 0, "Heater 3    ");
		OLED_print_xy(0, 1, "removed     ");
		break;
	case WARN_REMOVED4:
		OLED_print_xy(0, 0, "Heater 4    ");
		OLED_print_xy(0, 1, "removed     ");
		break;	
	case WARN_SHORTED:
		OLED_print_xy(0, 0, "Output      ");
		OLED_print_xy(0, 1, "shorted!    ");
		break;
	case WARN_OVERCURRENT:
		OLED_print_xy(0, 0, "Overcurrent ");
		OLED_print_xy(0, 1, "detected!   ");
		break;
	case WARN_HEATER_OVERCURRENT:
		OLED_print_xy(0, 0, "Heater      ");
		OLED_print_xy(0, 1, "overcurrent ");
		break;
	case WARN_VOLT_HIGH:
		OLED_print_xy(0, 0, "Supply volt.");
		OLED_print_xy(0, 1, "too high!   ");
		break;
	case WARN_VOLT_LOW:
		OLED_print_xy(0, 0, "Supply volt.");
		OLED_print_xy(0, 1, "too low!    ");
		break;
	case WARN_SENSOR_TIMEOUT:
		OLED_print_xy(0, 0, "Sensor      ");
		OLED_print_xy(0, 1, "timeout     ");
		break;
	case WARN_SENSOR_CHECKSUM:
		OLED_print_xy(0, 0, "Sensor wrong");
		OLED_print_xy(0, 1, "checksum    ");
		break;
	case ERR_NUKED:
		OLED_print_xy(0, 0, "OVERCURRENT ");
		OLED_print_xy(0, 1, "TURN OFF NOW");
		break;
	case ERR_VOLT_CRIT:
		OLED_print_xy(0, 0, "VOLTAGE HIGH");
		OLED_print_xy(0, 1, "TURN OFF NOW");
		/*
		INTCON = 0;
		OLED_command(OLED_CLEARDISPLAY);
		OLED_command(OLED_RETURNHOME);
		OLED_print_xy(0, 0, "TURNING OFF");
		setChannelSwitch(255, 0);
		PEN = 0;
		for(n = 5; n > 0; n--) {
			itoa(str, n, 1);
			OLED_print_xy(0, 1, "IN ");
			OLED_print_xy(3, 1, str);
			__delay_ms(1000);
		}
		OLED_off();
		OLED_PWR = 0;
		// TODO: Turn peripherals off, lower clock speed?
		while(1);
		 */
		break;
	case ERR_OVERCURRENT:
		OLED_print_xy(0, 0, "Overcurrent ");
		OLED_print_xy(0, 1, "Please check");
		if (getPB() == PB_SHORT)
			PEN = 1;
		break;
	case ERR_MENU:
		OLED_print_xy(0, 0, "Menu broken ");
		OLED_print_xy(0, 1, "    :-)     ");
		break;
	default:
		OLED_print_xy(0, 0, "Unknown     ");
		OLED_print_xy(0, 1, "error code  ");
	}
	
	if (getPB() == PB_SHORT) {
		if (head != tail)
			tail = (tail + 1) % NUM_ERROR_MESSAGES;
	}
	
	g_screenRefresh = 1;
}