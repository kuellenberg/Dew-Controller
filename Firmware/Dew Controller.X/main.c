	/*
 * File:   main.c
 * Author: Andre
 *
 * Created on 12. Januar 2020, 13:20
 */

#include "common.h"
#include "config.h"
#include "menu.h"
#include "oled.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TEMP_AUX_MIN -30
#define TEMP_AUX_MAX 100
#define SENSOR_UPDATE_INTERVALL 50
#define SENSOR_TIMEOUT 20

#define ALPHA(x) ( (uint32_t)(x * 65535) )

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void initialize(void);
void convertAnalogValues(t_globalData *data);
void checkSensor(t_globalData *data);
uint16_t ema(uint16_t in, uint16_t average, uint32_t alpha);
t_globalData data;
//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------
void main(void)
{		
	initialize();
	OLED_PWR = 1;
	PEN = 1;
	OLED_init();
	OLED_loadSpecialChars();
	OLED_returnHome();
	OLED_clearDisplay();
	
	SW_CH1 = 1;
	
	data.chData[0].lensDia = 4;
	data.chData[0].status = ON;
	data.chData[0].mode = AUTO;
	data.chData[0].Patt = 2.54;

	while (1) {
		CLRWDT();
		convertAnalogValues(&data);
		checkSensor(&data);
		menu(&data);
		__delay_ms(10);	
	}
}

//-----------------------------------------------------------------------------
// Test aux. temperature sensor, query main sensor, check data from sensor
//-----------------------------------------------------------------------------
void checkSensor(t_globalData *data)
{
	t_dataPacket *dp;
	static uint32_t sensorUpdateInterval = 0;
	static uint32_t sensorTimeout = 0;
	static uint8_t state = 0;	

	// Check aux. temperature sensor
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
				data->status.SENSOR_OK = 0;
				state = 0;
			} else if (uartIsDataReady()) {
				dp = getDataPacket(); // get Pointer to dataPacket
				if ((dp->header == 0xAA) && (dp->status == 1)) {
					data->tempC = dp->tempC;
					data->relHum = dp->relHum;
					data->dewPointC = dp->dewPointC;
					data->sensorVersion = dp->version;
					data->status.SENSOR_OK = 1;
				} else {
					// set error bits
					data->status.SENSOR_OK = 0;
				}
				state = 0;
			}
			break;
		default:
			state = 0;
	}
}

//-----------------------------------------------------------------------------
// Exponential moving average filter
//-----------------------------------------------------------------------------
uint16_t ema(uint16_t in, uint16_t average, uint32_t alpha)
{
  uint32_t tmp;
  tmp = in * alpha + average * (65536 - alpha);
  return (tmp + 32768) / 65536;
}
//-----------------------------------------------------------------------------
// Start ADC conversion and return result
//-----------------------------------------------------------------------------
uint16_t adcGetConversion(uint8_t channel)
{
	ADCON0bits.CHS = channel;
	__delay_us(5);
	ADCON0bits.GO = 1;
	while (ADCON0bits.GO);
	return (uint16_t)((ADRESH << 8) + ADRESL);
}
//-----------------------------------------------------------------------------
// Convert ADC counts to actual measurements
//-----------------------------------------------------------------------------
void convertAnalogValues(t_globalData *data)
{
	static uint16_t avgT, avgV, avgI;
	uint16_t adc;
	float f;
	
	adc = adcGetConversion(AIN_TEMP);
	avgT = ema(adc, avgT, ALPHA(0.65));
	adc = adcGetConversion(AIN_VSENS);
	avgV = ema(adc, avgV, ALPHA(0.65));
	adc = adcGetConversion(AIN_ISENS);
	avgI = ema(adc, avgI, ALPHA(0.65));
	data->tempAux = (avgT * 0.1191) - 34.512;
	data->voltage = ((float)avgV * 5.0 * (150.0 + 47.0)) / (1023.0 * 47.0);
	data->current = (avgI * 5.0) / (1023.0 * 0.05 * 50.0);
	data->power = data->voltage * data->current;
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------
void initialize(void)
{
	OSCFRQ = 0b00000010; // 4 MHz
	OSCCON1 = 0b01100000; // HINTOSC (1-32MHz), CDIV = 1
	while (!OSCCON3bits.ORDY); // Wait until clock switch is done

	// Peripheral Pin Select (PPS)
	RX1DTPPSbits.RX1DTPPS = 0x17;   //RC7->EUSART1:RX1;
	RC6PPS = 0x0F; //RC4->EUSART1:TX1;    

	// Analog/digital IO
	ANSELA = 0b01000000; // RA6: aux. temp. sensor analog input
	ANSELB = 0b00000000; // PORTB is digital only
	ANSELC = 0b00000011; // RC0: VSENS, RC1: ISENS

	// Data direction registers
	TRISA = 0b11110000; // Outputs: RA0..3: SW_CH1..4, Inputs: RA4..7
	TRISB = 0b00000000;
	TRISC = 0b10000111; // Inputs: RC7: RX, RC2: nFAULT, RC1: ISENS, RC0: VSENS

	// ADC
	ADCON0bits.ADON = 1;
	ADCON1 = 0b11100000; // Right justified, Fosc/64, Vref+=Vdd

	// Timer0: 10ms 
	T0CON0 = 0b10000000; // Enabled, 8-bit mode, postscaler 1:1
	T0CON1 = 0b01000111; // Clock Fosc/4, sync to Fosc, prescaler 1:128
	TMR0 = TMR0_PRELOAD;

	// Timer1: 100ms
	T1CON = 0b00110011; // Prescaler 1:8, 16-bit rd/wr, enabled
	T1CLK = 0b00000001; // Clock Fosc/4
	TMR1 = TMR1_PRELOAD;

	// Interrupts
	PIE0 = 0b00110000; // TMR0IE, IOCIE
	PIE3 = 0b00100000; // RC1IE
	PIE4 = 0b00000001; // TMR1IE
	INTCON = 0b11000000;    // GIE, PEIE

	// Interrupt-on-change
	IOCAP = 0b10110000; // Pos. edge on RA7, RA5, RA4 (PB, ROT_B, ROT_A)
	IOCAN = 0b10110000; // Neg. edge on RA7, RA5, RA4
	IOCCN = 0b00000100; // Neg. edge on RC2 (nFAULT)

	// EUSART
	// SYNC = 0, BRGH = 0, BRG16 = 1, SPBRG = 25 -> 9615 Baud (0.16% error)
	BAUD1CON = 0b00001000; // BRG16 = 1
	SPBRGL = 25;
	RC1STA = 0b10010000; // SPEN = 1, CREN = 1
	TX1STA = 0b00100000; // TXEN = 1
}