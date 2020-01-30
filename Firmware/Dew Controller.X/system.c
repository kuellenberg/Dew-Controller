#include "common.h"
#include "system.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TEMP_AUX_MIN -30
#define TEMP_AUX_MAX 100
#define SENSOR_UPDATE_INTERVALL 50
#define SENSOR_TIMEOUT 20
#define NUM_SAMPLES 10

#define MIN_CHANNEL_CURRENT 0.05
#define MAX_CHANNEL_CURRENT 2.0
#define MAX_CURRENT 3.0
#define VOLT_CRIT_HIGH 13.8
#define VOLT_WARN_HIGH 13.0
#define VOLT_WARN_LOW 11.4
#define VOLT_TURN_OFF 11.0

#define EPSILON 0.95
#define RHO 5.67e-8
#define PI 3.14
#define C_TO_K 273.15
#define INCH_TO_MM 0.0254
#define K_FACTOR 0.85
#define WIDTH 0.03

#define ADC_TO_I(counts) ( (counts * 5.0) / (1023.0 * 0.05 * 50.0) )
#define ADC_TO_V(counts) ( (counts * 5.0 * (150.0 + 47.0)) / (1023.0 * 47.0) )
#define ADC_TO_T(counts) ( (counts * 0.1191) - 34.512 )


typedef struct {
	int phyChNum;
	float current;
	uint8_t DC;
	uint8_t DCatt;
} t_virtChannel;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
t_virtChannel vChannels[NUM_CHANNELS];
uint8_t numGrpA, numGrpB;
int8_t grpA[NUM_CHANNELS], grpB[NUM_CHANNELS];


//-----------------------------------------------------------------------------
// Tests each channel: 
// - Is a heater connected?
// - Measure current through heater
// - Calculate max. power and required duty cycle
// 
// Returns '1' once data is ready
//-----------------------------------------------------------------------------

uint8_t checkChannelStatus(t_globalData *data)
{
	uint16_t adc;
	static uint16_t avg;
	static uint8_t channel = 0;
	static uint8_t samples = 0;
	static uint8_t ready = 0;
	float current;
	t_channelData *chData;

	if (ready) {
		ready = 0;
		samples = 0;
		channel = 0;
		avg = data->chData[channel].current;
	}
	
	chData = &data->chData[channel];
	
	if (chData->status == CH_OVERCURRENT) {
		if (++channel >= NUM_CHANNELS)
			ready = 1;
		return ready;
	}
	
	setChannelSwitch(channel, 1);
	// Not enough samples?
	if (samples++ < NUM_SAMPLES) {
		adc = getAnalogValue(AIN_ISENS);
		// Simple exp. moving average on raw value
		avg = ema(adc, avg, ALPHA(0.7));
	} else {
		setChannelSwitch(channel, 0);
		// convert raw value to actual current 
		current = ADC_TO_I(avg);
		// if current is below threshold, we assume
		// no heater is connected on this channel
		if (current < MIN_CHANNEL_CURRENT) {
			// Warning, if status has changed
			if (chData->status == CH_ENABLED)
				error(WARN_REMOVED);
			chData->status = CH_OPEN;
		} else if ((current > MAX_CHANNEL_CURRENT) || (! nFAULT)) {
			// Disable channel when current is too high
			// or load switch turned off 
			error(WARN_HEATER_OVERCURRENT);
			chData->status = CH_OVERCURRENT;
			// Reset loadswitch, if neccesary
			if (! nFAULT) {
				chData->status = CH_SHORTED;
				setLoadSwitch(0);
				__delay_ms(5);
				setLoadSwitch(1);
			}
		} else {
			chData->current = current;
			chData->Pmax = data->voltage * current;
			
			// Set status and mode
			if (chData->Pset > chData->Pmax)
				chData->Pset = chData->Pmax;
			
			if (data->status.SENSOR_OK) {
				if (chData->Pset < 0)
					chData->mode = MODE_AUTO;
				else if (chData->Pset > 0)
					chData->mode = MODE_MANUAL;
			} else {
				chData->Pset = chData->Pmax;
				chData->mode = MODE_MANUAL;
			}
			if (chData->Pset == 0)
				chData->status = CH_DISABLED;
			else
				chData->status = CH_ENABLED;
			
			if (chData->mode == MODE_AUTO)
				chData->DCreq = (chData->Preq / chData->Pmax) * 100;
			else 
				chData->DCreq = (chData->Pset / chData->Pmax) * 100;
		}
		// Next channel...
		if (channel < NUM_CHANNELS - 1) {
			channel++;
			samples = 0;
		} else {
			ready = 1;
		}
	}
	return ready;
}

//-----------------------------------------------------------------------------
// Perform system check: Overcurrent, battery voltage low/high
//-----------------------------------------------------------------------------

void systemCheck(t_globalData *data)
{
	uint8_t n;
	char str[3];
	
	// Max. current exceeded?
	// This condition 'should' only occur, if something shorts out during duty cycle
	if (data->current > MAX_CURRENT) {
		// Turn everything off
		setChannelSwitch(255, 0);
		setLoadSwitch(0);
		// Still overcurrent?
		if (ADC_TO_I(getAnalogValue(AIN_ISENS)) > MAX_CURRENT) {
			// ...ok, it's well and truely rooted 
			// it probably starts smoking soon
			error(ERR_NUKED);
		} else {
			// puh, just a dead short on the heater?
			if (! data->status.OVERCURRENT) {
				data->status.OVERCURRENT = 1;
				error(ERR_OVERCURRENT);
			}
		}
	}

	// The device is able to withstand voltages above 13.8V (up to 16V),
	// but we have to limit the current, otherwise the linear regulator
	// for the OLED display might get a little toasty.
	// If the voltage is too low, we might damage the battery.
	// So, in both cases, we just turn everything off.
	if ((data->voltage > VOLT_CRIT_HIGH) || 
			(data->voltage <= VOLT_TURN_OFF)) {
		INTCON = 0;
		OLED_clearDisplay();
		OLED_returnHome();
		OLED_print_xy(0, 0, "TURNING OFF");
		setChannelSwitch(255, 0);
		setLoadSwitch(0);
		for(n = 5; n > 0; n--) {
			itoa(str, n, 1);
			OLED_print_xy(0, 1, "IN ");
			OLED_print_xy(3, 1, str);
			__delay_ms(1000);
		}
		OLED_off();
		setOLEDPower(0);
		// TODO: Turn peripherals off, lower clock speed?
		
		while(1);
	} else if ((data->voltage > VOLT_WARN_HIGH) && (data->voltage <= VOLT_CRIT_HIGH)) {
		if (! data->status.BAT_HIGH) {
			data->status.BAT_HIGH = 1;
			error(WARN_VOLT_HIGH);
		}
	} else if ((data->voltage > VOLT_TURN_OFF) && (data->voltage <= VOLT_WARN_LOW)) {
		if (! data->status.BAT_LOW) {
			data->status.BAT_LOW = 1;
			error(WARN_VOLT_LOW);
		}
	} else {
		data->status.BAT_HIGH = 0;
		data->status.BAT_LOW = 0;
	}
}


//-----------------------------------------------------------------------------
// Test aux. temperature sensor, query main sensor, check data from sensor
//-----------------------------------------------------------------------------

uint8_t checkSensor(t_globalData *data)
{
	t_dataPacket *dp;
	static uint32_t sensorUpdateInterval = 0;
	static uint32_t sensorTimeout = 0;
	static uint8_t state = 0;

	// Check aux. temperature sensor and set status bit
	if ((data->tempAux < TEMP_AUX_MIN) || (data->tempAux > TEMP_AUX_MAX)) {
		data->status.AUX_SENSOR_OK = 0;
	} else
		data->status.AUX_SENSOR_OK = 1;

	switch (state) {
	case 0:
		// Request data from sensor after SENSOR_UPDATE_INTERVALL
		if (timeSince(sensorUpdateInterval) >= SENSOR_UPDATE_INTERVALL) {
			sensorUpdateInterval = sensorTimeout = timeNow();
			uartSendByte('?');
			state = 1;
		}
		break;
	case 1:
		// Wait for response
		if (timeSince(sensorTimeout) > SENSOR_TIMEOUT) {
			if (data->status.SENSOR_OK) {
				data->status.SENSOR_OK = 0;
				state = 0;
				error(WARN_SENSOR_TIMEOUT);
			}
			uartReset();
		} else if (uartIsDataReady()) {
			dp = getDataPacket(); // pointer to dataPacket
			// Is the header ok?
			if ((dp->header == 0xAA) && (dp->status == 1)) {
				data->tempC = dp->tempC;
				data->relHum = dp->relHum;
				data->dewPointC = dp->dewPointC;
				data->sensorVersion = dp->version;
				data->status.SENSOR_OK = 1;
				state = 0;
				
				return 1;
			} else {
				// Sensor not ok, set status bit and reset UART
				if (data->status.SENSOR_OK) {
					data->status.SENSOR_OK = 0;
					error(WARN_SENSOR_CHECKSUM);
				}
				uartReset();
			}
			state = 0;
		}
		break;
	default:
		state = 0;
	}
	
	return 0;
}


//-----------------------------------------------------------------------------
// Calculate the required power for each channel
// The calculation is based on the dew point temperature, lens diameter and 
// the heat transfer to the cold sky.
// We also try to estimate the required temperature at the heater,
// assuming that heat is only transferred through the lens :-)
//-----------------------------------------------------------------------------

void calcRequiredPower(t_globalData *data)
{
	uint8_t n;
	float d, A, T1, T2, phi;
	float p, Rth;

	for (n = 0; n < NUM_CHANNELS; n++) {
		// Calculate thermal radiation
		d = INCH_TO_MM * data->chData[n].lensDia; // Lens diameter in mm
		A = (PI * d * d) / 4; // Exposed area of the lens 
		// Assuming lens temperature has reached dew point + offset
		T1 = data->dewPointC + data->dpOffset + C_TO_K;
		T2 = data->skyTemp + C_TO_K;
		// Stefan Bolzman Law
		phi = EPSILON * RHO * A * (T1 * T1 * T1 * T1 - T2 * T2 * T2 * T2);
		// Required power is phi * experimental factor (heat loss etc...)
		data->chData[n].Preq = phi * data->fudgeFactor;

		// Approx. heater temp. required
		p = 2 * PI * INCH_TO_MM * data->chData[n].lensDia;
		A = p * WIDTH; // Area covered by heater strip
		// Thermal resistance of the lens
		Rth = (data->chData[n].lensDia / 2) * K_FACTOR * A;
		// Delta T
		data->chData[n].dt = phi * Rth - data->dewPointC;
	}
}

//-----------------------------------------------------------------------------
// Measure battery voltage, current and aux. temperature
//-----------------------------------------------------------------------------

void getAnalogValues(t_globalData *data)
{
	static uint16_t avgT, avgV, avgI;
	uint16_t adc;

	adc = getAnalogValue(AIN_TEMP);
	avgT = ema(adc, avgT, ALPHA(0.5));
	adc = getAnalogValue(AIN_VSENS);
	avgV = ema(adc, avgV, ALPHA(0.8));
	adc = getAnalogValue(AIN_ISENS);
	avgI = ema(adc, avgI, ALPHA(0.3));
	data->tempAux = ADC_TO_T(avgT);
	data->voltage = ADC_TO_V(avgV);
	data->current = ADC_TO_I(avgI);
	data->power = data->voltage * data->current;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
int sortDC(const void *cmp1, const void *cmp2)
{
	uint8_t a = *(uint8_t *)cmp1;
	uint8_t b = *(uint8_t *)cmp2;

	return (vChannels[b].DC - vChannels[a].DC);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
int sortDCRev(const void *cmp1, const void *cmp2)
{
	uint8_t a = *(uint8_t *)cmp1;
	uint8_t b = *(uint8_t *)cmp2;

	return (vChannels[a].DC - vChannels[b].DC);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
int sortCur(const void *cmp1, const void *cmp2)
{
	t_virtChannel *a = (t_virtChannel *)cmp1;
	t_virtChannel *b = (t_virtChannel *)cmp2;

	return (b->current - a->current);
}

//-----------------------------------------------------------------------------
// Creates virtual heater channels. If the sum of all heater currents exceed 
// the maximum allowed current of the device, an alternating load switching 
// scheme during duty cycle is determined.
//-----------------------------------------------------------------------------
void channelThing(t_globalData *data)
{	
	uint8_t n, phyCh;
	float total, totalGrpA, totalGrpB;
	
	for(n = 0; n < NUM_CHANNELS; n++) {
		grpA[n] = -1;
		grpB[n] = -1;
		
		vChannels[n].phyChNum = n;
		vChannels[n].current = data->chData[n].current;
		vChannels[n].DC = data->chData[n].DCreq;
	}
	
	qsort(vChannels, NUM_CHANNELS, sizeof(vChannels[0]), sortCur);
	
	total = totalGrpA = totalGrpB = 0;
	numGrpA = numGrpB = 0;
	
	for(n = 0; n < NUM_CHANNELS; n++) {
		
		vChannels[n].DCatt = vChannels[n].DC;
		total += vChannels[n].current;
		
		if (totalGrpA + vChannels[n].current <= MAX_CURRENT) {
			totalGrpA += vChannels[n].current;
			grpA[numGrpA++] = n;
		} else if (totalGrpB + vChannels[n].current <= MAX_CURRENT) {
			totalGrpB += vChannels[n].current;
			grpB[numGrpB++] = n;
		}
		
	}
	
	qsort(grpA, numGrpA, sizeof(grpA[0]), sortDC);
	qsort(grpB, numGrpB, sizeof(grpB[0]), sortDCRev);
	
	for(n = 0; n < numGrpA; n++) {
		if (grpB[n] > -1) {
			if (vChannels[grpA[n]].DC + vChannels[grpB[n]].DC > 100)
				vChannels[grpB[n]].DCatt = 100 - vChannels[grpA[n]].DC;
		}
	}
	
	for(n = 0; n < NUM_CHANNELS; n++) {
		phyCh = vChannels[n].phyChNum;
		
		data->chData[phyCh].Patt = (vChannels[n].DCatt * data->chData[phyCh].Pmax) / 100.0;
	}
}



