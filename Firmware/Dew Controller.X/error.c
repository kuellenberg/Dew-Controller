#include "common.h"

#define NUM_ERROR_MESSAGES 10

enum e_errorcode errorMessageQueue[NUM_ERROR_MESSAGES];
uint8_t head = 0;
uint8_t tail = 0;

void removeLastError(void);


void viewErrorMessage(void)
{
	enum e_errorcode errorCode = getLastError();
	
	switch(errorCode) {
	case WARN_REMOVED:
		OLED_print_xy(0, 0, "Heater      ");
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
	case WARN_VOLT_HIGH:
		OLED_print_xy(0, 0, "Supply volt.");
		OLED_print_xy(0, 1, "too high!   ");
		break;
	case WARN_VOLT_LOW:
		OLED_print_xy(0, 0, "Supply volt.");
		OLED_print_xy(0, 1, "too low!    ");
		break;
	case ERR_NUKED:
		OLED_print_xy(0, 0, "OVERCURRENT ");
		OLED_print_xy(0, 1, "TURN OFF NOW");
		break;
	case ERR_OVERCURRENT:
		OLED_print_xy(0, 0, "Overcurrent ");
		OLED_print_xy(0, 1, "Please check");
		break;
	case ERR_MENU:
		OLED_print_xy(0, 0, "Menu broken ");
		OLED_print_xy(0, 1, "    :-)     ");
		break;
	default:
		OLED_print_xy(0, 0, "Unknown     ");
		OLED_print_xy(0, 1, "error code  ");
	}
	
	if (getPB() == PB_SHORT)
		removeLastError();
}


void error(enum e_errorcode error)
{
	errorMessageQueue[head] = error;
	head = (head + 1) % NUM_ERROR_MESSAGES;
	if (head == tail)
		tail = (tail + 1) % NUM_ERROR_MESSAGES;
}

enum e_errorcode getLastError(void)
{
	enum e_errorcode e;
	
	if (head == tail)
		return NO_ERROR;
	return errorMessageQueue[tail];
	
	return e;		
}

void removeLastError(void)
{	
	if (head != tail)
		tail = (tail + 1) % LENGTH;
}