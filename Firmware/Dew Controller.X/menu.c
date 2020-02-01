#include "common.h"
#include "menuhelper.h"
#include "menu.h"

//-----------------------------------------------------------------------------
// Global variables 
//-----------------------------------------------------------------------------
static uint8_t selectedChannel = 0;

// number to ascii conversion buffer
static char str[10];

//-----------------------------------------------------------------------------
// Status screen: Temperature, humidity, dew point and battery voltage
//-----------------------------------------------------------------------------
uint8_t statusView(void)
{
	static uint8_t page = 0;
	static uint8_t prevSensorStat = 1;

	// Has sensor status changed since last call?
	if (prevSensorStat != data.status.SENSOR_OK) {
		prevSensorStat = data.status.SENSOR_OK;
		// with no sensor connected, only battery status is displayed
		if (data.status.SENSOR_OK)
			page = 0;
		else
			page = 3;
		g_screenRefresh = 1;
	}
	
	returnToPage(page);	
	if (g_screenRefresh) {
		OLED_print_xy(0, 0, "Temperature Rel.humidityDewpoint    Bat.   PowerVersion:");
		OLED_print_xy(4 * COLUMNS, 1, "Sensor:");
	}
	if (data.status.AUX_SENSOR_OK) {
		ftoa(str, data.tempC, 5, 1);
		OLED_print_xy(0, 1, str);
		OLED_print_xy(5, 1, "\001 \002");
		ftoa(str, data.tempAux, 3, 0);
		OLED_print_xy(8, 1, str);
		OLED_print_xy(11, 1, "\001");
	} else {
		ftoa(str, data.tempC, 5, 1);
		OLED_print_xy(0, 1, str);
		OLED_print_xy(5, 1, "\001      ");
	}
	ftoa(str, data.relHum, 5, 1);
	OLED_print_xy(COLUMNS, 1, str);
	OLED_print_xy(COLUMNS + 5, 1, "%     ");
	ftoa(str, data.dewPointC, 5, 1);
	OLED_print_xy(2 * COLUMNS, 1, str);
	OLED_print_xy(2 * COLUMNS + 5, 1, "\001      ");
	ftoa(str, data.voltage, 4, 1);
	OLED_print_xy(3 * COLUMNS, 1, str);
	OLED_print_xy(3 * COLUMNS + 4, 1, "V  ");
	ftoa(str, data.power, 4, 1);
	OLED_print_xy(3 * COLUMNS + 7, 1, str);
	OLED_print_xy(3 * COLUMNS + 11, 1, "W");
	itoa(str, VERSION, 3);
	OLED_print_xy(4 * COLUMNS + 8, 0, str);
	
	if (data.status.SENSOR_OK) {
		itoa(str, data.sensorVersion, 3);
		OLED_print_xy(4 * COLUMNS + 8, 1, str);
		page = paging(page, 5);
	}
	return page;
}

//-----------------------------------------------------------------------------
// Channel view: Lens diameter and output power / channel status
//-----------------------------------------------------------------------------
uint8_t channelView(void)
{
	static uint8_t page = 0;
	uint8_t n;

	returnToPage(page);
	if (g_screenRefresh) {
		for (n = 0; n < NUM_CHANNELS; n++) {
			OLED_print_xy(n * COLUMNS + 0, 0, "Ch ");
			itoa(str, n + 1, 1);
			OLED_print_xy(n * COLUMNS + 3, 0, str);
			OLED_print_xy(n * COLUMNS + 4, 0, ": ");
			itoa(str, data.heater[n].lensDia, 2);
			OLED_print_xy(n * COLUMNS + 6, 0, str);
			OLED_print_xy(n * COLUMNS + 8, 0, "\" ");
		}
	}

	for (n = 0; n < NUM_CHANNELS; n++) {
		switch (data.heater[n].status) {
		case CH_UNCHECKED:
			OLED_print_xy(n * COLUMNS + 0, 1, "Please wait!");
			break;
		case CH_DISABLED:
			OLED_print_xy(n * COLUMNS + 0, 1, "Disabled    ");
			break;
		case CH_ENABLED:
			ftoa(str, data.heater[n].Patt, 4, 1);
			//ftoa(str, data.chData[n].current, 6, 4);
			OLED_print_xy(n * COLUMNS + 0, 1, str);
			//OLED_print_xy(n * COLUMNS + 6, 1, "      ");
			//OLED_print_xy(n * COLUMNS + 4, 1, "W ");
			//OLED_print_xy(n * COLUMNS + 6, 1, (data.chData[n].mode == MODE_AUTO ? "auto  " : "manual"));
			if (data.heater[n].mode == MODE_AUTO) {
				OLED_print_xy(n * COLUMNS + 4, 1, " (");
				ftoa(str, data.heater[n].Preq, 4,1);
				OLED_print_xy(n * COLUMNS + 6, 1, str);
				OLED_print_xy(n * COLUMNS + 10, 1, "W)");
			} else {
				OLED_print_xy(n * COLUMNS + 4, 1, "W manual");
			}
			break;
		case CH_OPEN:
			OLED_print_xy(n * COLUMNS + 0, 1, "Disconnected");
			break;
		case CH_SHORTED:
			OLED_print_xy(n * COLUMNS + 0, 1, "Shorted!    ");
			break;
		case CH_OVERCURRENT:
			OLED_print_xy(n * COLUMNS + 0, 1, "Overcurrent!");
			break;
		default:
			OLED_print_xy(n * COLUMNS + 0, 1, "OndreSpecial");
			break;
		}
	}
	page = paging(page, 4);
	selectedChannel = page;

	return page;
}

//-----------------------------------------------------------------------------
// Channel setup menu: Output power (manual/auto) and lens diameter
//-----------------------------------------------------------------------------
uint8_t channelSetup(void)
{
	static uint8_t page = 0;
	t_heater *chData = &data.heater[selectedChannel];
	
	returnToPage(page);
	
	if ((chData->status == CH_ENABLED) || (chData->status == CH_DISABLED)) {	
		OLED_print_xy(0, 0, "Output power");
		if (chData->Pset == 0) {
			OLED_print_xy(0, 1, "Ch. ");
			itoa(str, selectedChannel + 1, 1);
			OLED_print_xy(4, 1, str);
			OLED_print_xy(5, 1, " off   ");
		} else if (chData->Pset < 0) {
			OLED_print_xy(0, 1, "Ch. ");
			itoa(str, selectedChannel + 1, 1);
			OLED_print_xy(4, 1, str);
			OLED_print_xy(5, 1, " auto  ");
		} else {
			ftoa(str, chData->Pset, 4, 1);
			OLED_print_xy(0, 1, str);
			OLED_print_xy(4, 1, "W manual");
		}

	} else if (chData->status == CH_OPEN) {

		OLED_print_xy(0, 0, "Not         ");
		OLED_print_xy(0, 1, "connected   ");

	} else if (chData->status == CH_UNCHECKED) {

		OLED_print_xy(0, 0, "Testing     ");
		OLED_print_xy(0, 1, "heater...   ");

	} else if ((chData->status == CH_OVERCURRENT) || (chData->status == CH_SHORTED)) {

		OLED_print_xy(0, 0, "Reset       ");
		OLED_print_xy(0, 1, "channel     ");

	}		

	OLED_print_xy(COLUMNS, 0, "Lens diam.  ");
	ftoa(str, chData->lensDia, 4, 1);
	OLED_print_xy(COLUMNS + 0, 1, str);
	OLED_print_xy(COLUMNS + 5, 1, " inch  ");
			
	page = paging(page, 2);
	
	return page;
}

//-----------------------------------------------------------------------------
// Set output power level
//-----------------------------------------------------------------------------
uint8_t setOutputPower(void)
{
	t_heater *chData = &data.heater[selectedChannel];

	returnToPage(0);
	
	if ((chData->status == CH_ENABLED) || (chData->status == CH_DISABLED)) {
	
		if (g_screenRefresh)
			OLED_print_xy(0, 0, "Output power");
		spinInput(&chData->Pset, -0.25, chData->Pmax, 0.25);
		if (chData->Pset == 0) {
			itoa(str, selectedChannel + 1, 1);
			OLED_print_xy(0, 1, "\004Ch. ");
			OLED_print_xy(5, 1, str);
			OLED_print_xy(6, 1, " off \003");
		} else if (chData->Pset < 0) {
			itoa(str, selectedChannel + 1, 1);
			OLED_print_xy(0, 1, "\004Ch. ");
			OLED_print_xy(5, 1, str);
			OLED_print_xy(6, 1, " auto\003");
		} else {
			OLED_print_xy(0, 1, "\004");
			ftoa(str, chData->Pset, 4, 1);
			OLED_print_xy(1, 1, str);
			OLED_print_xy(5, 1, "W man.\003");
		}
		
	} else if ((chData->status == CH_OVERCURRENT) || (chData->status == CH_SHORTED)) {
		
		if (g_screenRefresh) {
			OLED_print_xy(0, 0, "Hold button ");
			OLED_print_xy(0, 1, "to re-enable");
		}
		if (getPB() == PB_LONG)
			chData->status = CH_UNCHECKED;
		return 0;
		
	} 
	else 
		return 1;

	return 0;
}

//-----------------------------------------------------------------------------
// Set lens diameter
//-----------------------------------------------------------------------------
uint8_t setLensDia(void)
{
	returnToPage(0);
	if (g_screenRefresh)
		OLED_print_xy(0, 0, "Lens diam.  ");
	spinInput(&data.heater[selectedChannel].lensDia, 1, 16, 0.5);
	ftoa(str, data.heater[selectedChannel].lensDia, 4, 1);
	OLED_print_xy(0, 1, "\004");
	OLED_print_xy(1, 1, str);
	OLED_print_xy(5, 1, " inch \003");
	return 0;
}

//-----------------------------------------------------------------------------
// Setup menu
//-----------------------------------------------------------------------------
uint8_t setup(void)
{
	static uint8_t page = 0;

	returnToPage(page);
	if (g_screenRefresh) {
		OLED_print_xy(0, 0, "DP offset   Sky temp.   Fudge factor");
		OLED_print_xy(0, 1, "temp. ");
		ftoa(str, data.dpOffset, 4, 1);
		OLED_print_xy(6, 1, str);
		OLED_print_xy(10, 1, "\001 ");
		ftoa(str, data.skyTemp, 3, 0);
		OLED_print_xy(COLUMNS + 0, 1, str);
		OLED_print_xy(COLUMNS + 3, 1, "\001        ");
		ftoa(str, data.fudgeFactor, 3, 1);
		OLED_print_xy(2 * COLUMNS + 0, 1, str);
		OLED_print_xy(2 * COLUMNS + 4, 1, "         ");
	}
	page = paging(page, 3);
	return page;
}
//-----------------------------------------------------------------------------
// Set dewpoint offset temperature
//-----------------------------------------------------------------------------
uint8_t setDPOffset(void)
{
	returnToPage(0);
	if (g_screenRefresh)
		OLED_print_xy(0, 0, "DP offset   ");
	spinInput(&data.dpOffset, 0, 10, 0.5);
	ftoa(str, data.dpOffset, 4, 1);
	OLED_print_xy(0, 1, "temp.\004");
	OLED_print_xy(6, 1, str);
	OLED_print_xy(10, 1, "\001\003");
	return 0;
}

//-----------------------------------------------------------------------------
// Set sky temperature
//-----------------------------------------------------------------------------

uint8_t setSkyTemp(void)
{
	returnToPage(0);
	if (g_screenRefresh)
		OLED_print_xy(0, 0, "Sky temp.   ");
	spinInput(&data.skyTemp, -50, -20, 1);
	ftoa(str, data.skyTemp, 3, 0);
	OLED_print_xy(0, 1, "\004");
	OLED_print_xy(1, 1, str);
	OLED_print_xy(4, 1, "\001 \003     ");
	return 0;
}

//-----------------------------------------------------------------------------
// Set fudge factor
//-----------------------------------------------------------------------------

uint8_t setFudgeFactor(void)
{
	returnToPage(0);
	if (g_screenRefresh)
		OLED_print_xy(0, 0, "Fudge factor");
	spinInput(&data.fudgeFactor, 0.1, 5.0, 0.1);
	ftoa(str, data.fudgeFactor, 3, 1);
	OLED_print_xy(0, 1, "\004");
	OLED_print_xy(1, 1, str);
	OLED_print_xy(4, 1, " \003      ");
	return 0;
}