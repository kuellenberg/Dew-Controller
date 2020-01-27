#include "common.h"
#include "menuhelper.h"
#include "menu.h"
#include "oled.h"
#include "inputs.h"

//-----------------------------------------------------------------------------
// Global variables 
//-----------------------------------------------------------------------------
static uint8_t selectedChannel = 0;

// long and short screen buffer
static char lBuf[SCREEN_BUFFER_SIZE], sBuf[COLUMNS + 1]; 

//-----------------------------------------------------------------------------
// Status screen: Temperature, humidity, dew point and battery voltage
//-----------------------------------------------------------------------------
uint8_t statusView(t_globalData *data)
{
	static uint8_t page = 0;
	float f;
	
	returnToPage(page);
	if (data->status.SENSOR_OK) {
		OLED_print_xy(0, 0, "Temperature Rel.humidityDewpoint    Bat.   Power");
		if (data->status.AUX_SENSOR_OK)
			sprintf(sBuf, "%3.1f\001\002%3.1f\001", data->tempC, data->tempAux);
		else
			sprintf(sBuf, "%5.1f\001      ", data->tempC);
		sprintf(lBuf, "%s%5.1f %%     %5.1f\001      %4.1fV  %4.1fW", 
			sBuf, data->relHum, data->dewPointC, data->voltage, data->power);
		OLED_print_xy(0, 1, lBuf);
		page = paging(page, 4);
	} else {
		page = 0;
		OLED_returnHome();
		OLED_print_xy(0, 0, "Bat.   Power");
		f = data->voltage;
		sprintf(sBuf, "%4.1fV", 9.997296);
		OLED_print_xy(0, 1, sBuf);
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
			sprintf(sBuf, "Ch %1d: %4.1f\" ", n + 1, data->chData[n].lensDia);
			strcat(lBuf,sBuf);
		}
		OLED_print_xy(0, 0, lBuf);
	}
	
	lBuf[0] = '\0';
	for(n = 0; n < NUM_CHANNELS; n++) {
		switch(data->chData[n].status) {
		case CH_DISABLED:
			strcpy(sBuf, "Off         ");
			break;
		case CH_ENABLED:
			tmp0 = data->chData[n].Patt;
			sprintf(sBuf, "%4.1fW %s", tmp0, (data->chData[n].mode == MODE_AUTO ? "auto  " : "manual"));
			break;
		case CH_OPEN:
			strcpy(sBuf, "Disconnected");
			break;
		case CH_SHORTED:
			strcpy(sBuf, "Shorted!    ");
			break;
		case CH_OVERCURRENT:
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
	selectedChannel = page;
	
	return page;
}

//-----------------------------------------------------------------------------
// Channel setup menu: Output power (manual/auto) and lens diameter
//-----------------------------------------------------------------------------
uint8_t channelSetup(t_globalData *data)
{
	static uint8_t page = 0;
	t_channelData *chData = &data->chData[selectedChannel];
	
	returnToPage(page);
	OLED_print_xy(0, 0, "Output powerLens diam.  ");
	
	if (chData->Preq == 0)
		sprintf(lBuf, "Ch. %1d off   ", selectedChannel + 1);
	else if (chData->Preq == chData->Pmax)
		sprintf(lBuf, "Ch. %1d auto  ", selectedChannel + 1);
	else
		sprintf(lBuf, "%4.fW manual", chData->Preq);

	sprintf(sBuf, "%4.1f inch   ", chData->lensDia);
	strcat(lBuf, sBuf);

	OLED_print_xy(0, 1, lBuf);
	
	page = paging(page, 2);
	return page;
}

//-----------------------------------------------------------------------------
// Set output power level
//-----------------------------------------------------------------------------
uint8_t setOutputPower(t_globalData *data)
{
	t_channelData *chData = &data->chData[selectedChannel];
	
	returnToPage(0);
	OLED_print_xy(0, 0, "Output power");
	spinInput(&chData->Preq, 0, chData->Pmax, 0.25);
	if (chData->Preq == 0)
		sprintf(sBuf, "Ch. %1d off   ", selectedChannel + 1);
	else if (chData->Preq == chData->Pmax)
		sprintf(sBuf, "Ch. %1d auto  ", selectedChannel + 1);
	else
		sprintf(sBuf, "%4.1fW manual", chData->Preq);
	OLED_print_xy(0,1,sBuf);
	return 0;
}

//-----------------------------------------------------------------------------
// Set lens diameter
//-----------------------------------------------------------------------------
uint8_t setLensDia(t_globalData *data)
{
	returnToPage(0);
	OLED_print_xy(0, 0, "Lens diam.  ");
	spinInput(&data->chData[selectedChannel].lensDia, 1, 16, 0.5);
	sprintf(sBuf, "%4.1f inch   ", data->chData[selectedChannel].lensDia);
	OLED_print_xy(0, 1, sBuf);
	return 0;
}

//-----------------------------------------------------------------------------
// Setup menu
//-----------------------------------------------------------------------------
uint8_t setup(t_globalData *data)
{
	static uint8_t page = 0;
	
	returnToPage(page);
	OLED_print_xy(0, 0, "DP offset   Sky temp.   Fudge factorBack");
	sprintf(lBuf, "temp. +3.1f\001%3.0f\001        %3.1f         \003           ", 
		data->dpOffset, data->skyTemp, data->fudgeFactor);
	OLED_print_xy(0, 1, lBuf);
	page = paging(page, 4);
	return page;
}

//-----------------------------------------------------------------------------
// Set dewpoint offset temperature
//-----------------------------------------------------------------------------
uint8_t setDPOffset(t_globalData *data)
{
	returnToPage(0);
	OLED_print_xy(0, 0, "DP offset   ");
	spinInput(&data->dpOffset, 0, 10, 0.5);
	sprintf(sBuf, "temp. +3.1f\001", data->dpOffset);
	OLED_print_xy(0,1,sBuf);
	return 0;
}

//-----------------------------------------------------------------------------
// Set sky temperature
//-----------------------------------------------------------------------------
uint8_t setSkyTemp(t_globalData *data)
{
	returnToPage(0);
	OLED_print_xy(0, 0, "Sky temp.   ");
	spinInput(&data->skyTemp, -50, -20, 1);
	sprintf(sBuf, "%3.0f\001        ", data->skyTemp);
	OLED_print_xy(0,1,sBuf);
	return 0;
}

//-----------------------------------------------------------------------------
// Set fudge factor
//-----------------------------------------------------------------------------
uint8_t setFudgeFactor(t_globalData *data)
{
	returnToPage(0);
	OLED_print_xy(0, 0, "Fudge factor");
	spinInput(&data->fudgeFactor, 0.1, 5, 0.1);
	sprintf(sBuf, "%3.1f         ", data->fudgeFactor);
	OLED_print_xy(0,1,sBuf);
	return 0;
}