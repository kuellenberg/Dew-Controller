#include "common.h"
#include "system.h"
#include "memory.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TEMP_AUX_MIN -30
#define TEMP_AUX_MAX 60
#define SENSOR_UPDATE_INTERVALL 100
#define SENSOR_TIMEOUT 20
#define NUM_SAMPLES 50

#define MIN_CHANNEL_CURRENT 0.05
#define MAX_CHANNEL_CURRENT 2.0
#define MAX_CURRENT 3.5
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
	int phyChanNum;
	float current;
	uint8_t DC;
	uint8_t start;
	uint8_t stop;
} t_virtChannel;

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static t_virtChannel virtChannels[NUM_CHANNELS];


//-----------------------------------------------------------------------------
// Tests each channel: 
// - Is a heater connected?
// - Measure current through heater
// - Calculate max. power and required duty cycle
//-----------------------------------------------------------------------------

void checkChannelStatus(void)
{
	uint16_t adc, avg;
	uint8_t channel, samples;
	float current;
	t_heater *heater;

	for (channel = 0; channel < NUM_CHANNELS; channel++)  {
		
		heater = &data.heater[channel];
		
		if (heater->status == CH_OVERCURRENT) 
			continue;
		
		avg = data.heater[channel].current;

		setChannelSwitch(channel, 1);		
		samples = 0;
		do {
			adc = getAnalogValue(AIN_ISENS);
			// Calculate exp. moving average on raw value
			avg = ema(adc, avg, ALPHA(0.7));
		} while (samples++ < NUM_SAMPLES);		
		setChannelSwitch(channel, 0);
		
		// convert raw value into actual current 
		current = ADC_TO_I(avg);
		// if current is below threshold, we assume
		// no heater is connected to this channel
		if (current < MIN_CHANNEL_CURRENT) {
			// Warning, if channel as previously enabled
			if (heater->status == CH_ENABLED)
				error(WARN_REMOVED);
			heater->status = CH_OPEN;
		} else if ((current > MAX_CHANNEL_CURRENT) || !nFAULT) {
			// Disable channel when current is too high
			// or load switch is turned off 
			error(WARN_HEATER_OVERCURRENT);
			heater->status = CH_OVERCURRENT;
			// Reset loadswitch, if neccesary
			if (!nFAULT) {
				heater->status = CH_SHORTED;
				PEN = 0;
				__delay_ms(5);
				PEN = 1;
			}
		} else {
			heater->current = current;
			heater->Pmax = data.voltage * current;

			// Set status and mode
			if (heater->Pset > heater->Pmax)
				heater->Pset = heater->Pmax;

			if (data.status.SENSOR_OK) {
				if (heater->Pset < 0)
					heater->mode = MODE_AUTO;
				else if (heater->Pset > 0)
					heater->mode = MODE_MANUAL;
			} else {
				heater->Pset = heater->Pmax;
				heater->mode = MODE_MANUAL;
			}

			if (heater->Pset == 0)
				heater->status = CH_DISABLED;
			else
				heater->status = CH_ENABLED;

			// Calculate required duty cycle
			if (heater->mode == MODE_AUTO)
				heater->DCreq = MIN((heater->Preq / heater->Pmax) * 100, 100);
			else 
				heater->DCreq = MIN((heater->Pset / heater->Pmax) * 100, 100);
		}
	}
}

//-----------------------------------------------------------------------------
// Perform system check: Overcurrent, battery voltage low/high
//-----------------------------------------------------------------------------

void systemCheck(void)
{
	uint8_t n;
	char str[3];
	
	// Max. current exceeded?
	// This condition 'should' only occur, if something shorts out during duty cycle
	if (data.current > MAX_CURRENT) {
		// Turn everything off
		setChannelSwitch(255, 0);
		PEN = 0;
		// Still overcurrent?
		if (ADC_TO_I(getAnalogValue(AIN_ISENS)) > MAX_CURRENT) {
			// ...ok, it's well and truely rooted 
			// it probably starts smoking soon
			error(ERR_NUKED);
		} else {
			// puh, just a dead short on the heater?
			if (! data.status.OVERCURRENT) {
				data.status.OVERCURRENT = 1;
				error(ERR_OVERCURRENT);
			}
		}
	}

	// The device is able to withstand voltages above 13.8V (up to 16V),
	// but we have to limit the current, otherwise the linear regulator
	// for the OLED display might get a little toasty.
	// If the voltage is too low, we might damage the battery.
	// So, in both cases, we just turn everything off.
	if ((data.voltage > VOLT_CRIT_HIGH) || (data.voltage <= VOLT_TURN_OFF)) {
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
		
	} else if ((data.voltage > VOLT_WARN_HIGH) && (data.voltage <= VOLT_CRIT_HIGH)) {
		if (! data.status.BAT_HIGH) {
			data.status.BAT_HIGH = 1;
			error(WARN_VOLT_HIGH);
		}
	} else if ((data.voltage > VOLT_TURN_OFF) && (data.voltage <= VOLT_WARN_LOW)) {
		if (! data.status.BAT_LOW) {
			data.status.BAT_LOW = 1;
			error(WARN_VOLT_LOW);
		}
	} else {
		data.status.BAT_HIGH = 0;
		data.status.BAT_LOW = 0;
	}
}


//-----------------------------------------------------------------------------
// Test aux. temperature sensor, query main sensor, check data from sensor
//-----------------------------------------------------------------------------

uint8_t checkSensor(void)
{
	t_dataPacket *dp;
	static uint32_t sensorUpdateInterval = SENSOR_UPDATE_INTERVALL;
	static uint32_t sensorTimeout = 0;
	static uint8_t state = 0;

	// Check aux. temperature sensor and set status bit
	if ((data.tempAux < TEMP_AUX_MIN) || (data.tempAux > TEMP_AUX_MAX)) {
		data.status.AUX_SENSOR_OK = 0;
	} else
		data.status.AUX_SENSOR_OK = 1;

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
			if (data.status.SENSOR_OK) {
				data.status.SENSOR_OK = 0;
				state = 0;
				error(WARN_SENSOR_TIMEOUT);
			}
			uartReset();
		} else if (uartDataReadyFlag) {
			uartDataReadyFlag = 0;
			// Is the header ok?
			if ((dataPacket.header == 0xAA) && (dataPacket.status == 1)) {
				data.tempC = dataPacket.tempC;
				data.relHum = dataPacket.relHum;
				data.dewPointC = dataPacket.dewPointC;
				data.sensorVersion = dataPacket.version;
				data.status.SENSOR_OK = 1;
				state = 0;
				
				return 1;
			} else {
				// Sensor not ok, set status bit and reset UART
				if (data.status.SENSOR_OK) {
					data.status.SENSOR_OK = 0;
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

void calcRequiredPower(void)
{
	uint8_t n;
	float d, A, T1, T2, phi;
	float p, Rth;

	for (n = 0; n < NUM_CHANNELS; n++) {
		
		// If ambient temperure is above dew point + offset, no heating is required
		/*
		if (data.tempC > data.dewPointC + data.dpOffset) {
			data.heater[n].Preq = 0;
			continue;
		}
		*/
		// Calculate thermal radiation
		d = INCH_TO_MM * data.heater[n].lensDia; // Lens diameter in mm
		A = (PI * d * d) / 4; // Exposed area of the lens 
		// Assuming lens temperature has reached dew point + offset
		T1 = data.dewPointC + data.dpOffset + C_TO_K;
		T2 = data.skyTemp + C_TO_K;
		// Stefan Bolzman Law
		phi = EPSILON * RHO * A * (T1 * T1 * T1 * T1 - T2 * T2 * T2 * T2);
		// Required power is phi * experimental factor (heat loss etc...)
		data.heater[n].Preq = phi * data.fudgeFactor;

		// Approx. heater temp. required
		p = 2 * PI * INCH_TO_MM * data.heater[n].lensDia;
		A = p * WIDTH; // Area covered by heater strip
		// Thermal resistance of the lens
		Rth = (data.heater[n].lensDia / 2) * K_FACTOR * A;
		// Delta T
		data.heater[n].dt = phi * Rth - data.dewPointC;
	}
}

//-----------------------------------------------------------------------------
// Measure battery voltage, current and aux. temperature
//-----------------------------------------------------------------------------

void getAnalogValues(void)
{
	static uint16_t avgT, avgV, avgI;
	uint16_t adc;

	adc = getAnalogValue(AIN_TEMP);
	avgT = ema(adc, avgT, ALPHA(0.5));
	adc = getAnalogValue(AIN_VSENS);
	avgV = ema(adc, avgV, ALPHA(0.8));
	adc = getAnalogValue(AIN_ISENS);
	avgI = ema(adc, avgI, ALPHA(0.3));
	data.tempAux = ADC_TO_T(avgT);
	data.voltage = ADC_TO_V(avgV);
	data.current = ADC_TO_I(avgI);
	data.power = data.voltage * data.current;
}

//-----------------------------------------------------------------------------
// Sort virtual channels by duty cycle, ascending order
//-----------------------------------------------------------------------------
int sortDC(const void *cmp1, const void *cmp2)
{
	uint8_t a = *(uint8_t *)cmp1;
	uint8_t b = *(uint8_t *)cmp2;

	return (virtChannels[b].DC - virtChannels[a].DC);
}

//-----------------------------------------------------------------------------
// Sort virtual channels by duty cycle, descending order
//-----------------------------------------------------------------------------
int sortDCRev(const void *cmp1, const void *cmp2)
{
	uint8_t a = *(uint8_t *)cmp1;
	uint8_t b = *(uint8_t *)cmp2;

	return (virtChannels[a].DC - virtChannels[b].DC);
}

//-----------------------------------------------------------------------------
// Sort virtual channels by current
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
void channelThing(void)
{	
	uint8_t n;
	float total, totalGrpA, totalGrpB;
	uint8_t numGrpA, numGrpB;
	int8_t grpA[NUM_CHANNELS], grpB[NUM_CHANNELS];

	// Copy physical channel data into 'virtual channal' array
	for(n = 0; n < NUM_CHANNELS; n++) {
		grpA[n] = -1;
		grpB[n] = -1;
		
		virtChannels[n].phyChanNum = n;
		virtChannels[n].current = data.heater[n].current;
		virtChannels[n].DC = data.heater[n].DCreq;
	}
	
	// Sort virtual channel array by current
	qsort(virtChannels, NUM_CHANNELS, sizeof(virtChannels[0]), sortCur);
	
	total = totalGrpA = totalGrpB = 0;
	numGrpA = numGrpB = 0;
	
	// Split channels into two groups. Add channels to group A until max.
	// total current is reached. The remaining channels go into group B.
	for(n = 0; n < NUM_CHANNELS; n++) {		
		total += virtChannels[n].current;		
		if (totalGrpA + virtChannels[n].current <= MAX_CURRENT) {
			totalGrpA += virtChannels[n].current;
			grpA[numGrpA++] = n;
		} else if (totalGrpB + virtChannels[n].current <= MAX_CURRENT) {
			totalGrpB += virtChannels[n].current;
			grpB[numGrpB++] = n;
		}		
	}
	
	// Sort groups by duty cycle, A in descending order, B in ascending order
	qsort(grpA, numGrpA, sizeof(grpA[0]), sortDC);
	qsort(grpB, numGrpB, sizeof(grpB[0]), sortDCRev);
	
	// If duty cycle times in group A and B overlap, duty cycle in group B is reduced
	for(n = 0; n < numGrpA; n++) {
		if (grpB[n] > -1) {
			if (virtChannels[grpA[n]].DC + virtChannels[grpB[n]].DC > 100)
				virtChannels[grpB[n]].DC = 100 - virtChannels[grpA[n]].DC;
		}
		// The controller uses start and stop times to switch the heaters on/off.
		// We simply use the duty cycle value as on-time.
		virtChannels[grpA[n]].start = 0;
		virtChannels[grpA[n]].stop = virtChannels[grpA[n]].DC;
	}
	// Start and stop times for group B
	for(n = 0; n < numGrpB; n++) {
		virtChannels[grpB[n]].start = 100 - virtChannels[grpB[n]].DC ;
		virtChannels[grpB[n]].stop = 100;
	}
	
	// Attained power levels are calculated from new duty cycle values
	for(n = 0; n < NUM_CHANNELS; n++) {
		data.heater[virtChannels[n].phyChanNum].Patt = 
			(virtChannels[n].DC * data.heater[virtChannels[n].phyChanNum].Pmax) / 100.0;
	}
}


//-----------------------------------------------------------------------------
// Heater control loop
//-----------------------------------------------------------------------------
uint8_t controller(void)
{
	static uint32_t dutyCycleTimer;
	uint32_t tick;
	static uint8_t idle;
	uint8_t n;
	
	if (idle) {
		dutyCycleTimer = timeNow();
		idle = 0;
	}
		
	tick = timeSince(dutyCycleTimer);
	if (tick <= 100) {
		for(n = 0; n < NUM_CHANNELS; n++) {
			if ((tick >= virtChannels[n].start) && (tick < virtChannels[n].stop))
				setChannelSwitch(virtChannels[n].phyChanNum, 1);
			else
				setChannelSwitch(virtChannels[n].phyChanNum, 0);
		}
		NOP();
	} else {
		idle = 1;
	}
	
	return idle;
}


uint8_t storeNVM(void)
{
	uint8_t n;
	t_nvmData nvm;
	uint16_t buf[32];
	
	nvm.lensDia[0] = data.heater[0].lensDia;
	nvm.lensDia[1] = data.heater[1].lensDia;
	nvm.lensDia[2] = data.heater[2].lensDia;
	nvm.lensDia[3] = data.heater[3].lensDia;
	nvm.dpOffset = data.dpOffset;
	nvm.skyTemp = data.skyTemp;
	nvm.fudgeFactor = data.fudgeFactor;
	
	for(n = 0; n < 32; n++) {
		buf[n] = (uint16_t)nvm.raw[n];
	}
	
	FLASH_EraseBlock(0x3f80);
	FLASH_WriteBlock(0x3f80, buf);
	
	return 0;
}


void readNVM(void)
{
	uint8_t n;
	t_nvmData nvm;
	uint16_t buf[32];
	
	for(n = 0; n < 32; n++) {
		buf[n] = FLASH_ReadWord(0x3F80 + n);
		nvm.raw[n] = (uint8_t)(buf[n] & 0x00ff);
	}
	
	data.heater[0].lensDia = nvm.lensDia[0];
	data.heater[1].lensDia = nvm.lensDia[1];
	data.heater[2].lensDia = nvm.lensDia[2];
	data.heater[3].lensDia = nvm.lensDia[3];
	data.dpOffset = nvm.dpOffset;
	data.skyTemp = nvm.skyTemp;
	data.fudgeFactor = nvm.fudgeFactor;
}