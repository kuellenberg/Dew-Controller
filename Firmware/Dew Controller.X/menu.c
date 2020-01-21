#include "common.h"
#include "menu.h"
#include "oled.h"
#include "inputs.h"

#define COLUMNS 12

//-----------------------------------------------------------------------------
// Main menu screen
//-----------------------------------------------------------------------------
void menu(t_globalData *data)
{
	static uint8_t menu = 0; // Menu screen
	static uint8_t page = 0; // Page number (max. 4 pages per menu screen)
	char s[61], s12[12];

	switch(menu) {
	case 0:
		// Main menu
		if (data->status.SENSOR_OK) {
			OLED_print_xy(0, 0, "Temperature Rel.humidityDewpoint    Bat.   Power");
			if (data->status.AUX_SENSOR_OK)
				sprintf(s12, "%3.0f | %3.0f \337C", data->tempC, data->tempAux);
			else
				sprintf(s12, "%5.1f \337C    ", data->tempC);
			sprintf(s, "%s%5.1f %%     %5.1f \337C    %4.1fV  %4.1fW", 
				s12, data->relHum, data->dewPointC, data->voltage, data->power);
			OLED_print_xy(0, 1, s);
			menuInput(&page, 4, &menu, 1, 0, 0);
		} else {
			page = 0;
			OLED_returnHome();
			OLED_print_xy(0, 0, "Bat.   Power");
			sprintf(s, "%4.1fV  %4.1fW", data->voltage, data->power);
			OLED_print_xy(0, 1, s);
			menuInput(&page, 1, &menu, 1, 0, 0);
		}
		break;
		
	case 1:
		// Display power for each channel
		OLED_print_xy(0, 0, "Ch1: xx inchCh2: xx inchCh3: xx inchCh4: xx inch");
		menuInput(&page, 4, &menu, 1, 0, 0);
		break;
	default:
		menu = 0;
	}
}
//-----------------------------------------------------------------------------
// Handle input 
//-----------------------------------------------------------------------------
void menuInput(uint8_t *page, const uint8_t numPages, uint8_t *menu, 
	uint8_t pbShort, uint8_t pbLong, uint8_t timeout)
{
	uint8_t n;
	enum e_direction dir;
	enum e_buttonPress pb;
	
	PIE0bits.IOCIE = 0;
	dir = getRotDir();
	pb = getPB();
	
	if ((dir == ROT_CW) && (*page < numPages - 1)) {
		(*page)++;
		for(n = 0; n < COLUMNS; n++) {
			OLED_scrollDisplayLeft();
			__delay_ms(20);
		}
	}
	else if ((dir == ROT_CCW) && (*page > 0)) {
		(*page)--;
		for(n = 0; n < COLUMNS; n++) {
			OLED_scrollDisplayRight();
			__delay_ms(20);
		}
	}
	if (pb == PB_SHORT) {
		*menu = pbShort;
		*page = 0;
		OLED_returnHome();
	} else if (pb == PB_LONG) {
		*menu = pbLong;
		*page = 0;
		OLED_returnHome();
	}
	PIE0bits.IOCIE = 1;
}
