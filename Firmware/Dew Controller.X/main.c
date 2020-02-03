/*
 * File:   main.c
 * Author: Andre
 *
 * Created on 12. Januar 2020, 13:20
 */
#include "common.h"
#include "io.h"
#include "config.h"
#include "menuhelper.h"
#include "system.h"
#include "memory.h"
#include "oled.h"
#include "interrupt.h"
#include "error.h"
#include "interrupt.h"
#include "inputs.h"

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void initialize(void);
void initGlobalData(void);

//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------

void main(void)
{
	uint32_t sysCheckInterval = 0;
	uint8_t controllerIdle = 1;
	uint8_t initDone = 0;
	uint8_t displayOff = 0;

	initialize();
	OLED_PWR = 1;
	OLED_init();
	OLED_loadSpecialChars();
	OLED_command(OLED_RETURNHOME);
	OLED_command(OLED_CLEARDISPLAY);
	initGlobalData();
	PEN = 1;
	
	// Skip reading configuration from flash by pressing button during power up
	if (ROT_PB)
		readNVM();
	
	while (1) {
		// clear watchdog timer TODO: setup watchdog timer properly
		CLRWDT();

		// get battery voltage, current and aux. temperature
		getAnalogValues();
		// system check every 0.5s
		if (timeSince(sysCheckInterval) > 5) {
			sysCheckInterval = timeNow();
			systemCheck();
		}

		// query sensor box
		if (checkSensor()) {
			// once new sensor data is ready, calculate required heater power
			calcRequiredPower();
			initDone = 1;
			if ((data.status.SENSOR_OK) && (data.tempC < 1))
				NOP();
		}


		// is control loop running?
		if (controllerIdle) {
			if (initDone) {
				// Wait until initial sensor check is finished		
				// TODO: interval ?				
				checkChannelStatus();
				channelThing();
				controllerIdle = 0;
			}
		} else {
			// controller returns true after each cycle
			controllerIdle = controller();
		}
		
		if (getLastError() != NO_ERROR) {
			userActivity = timeNow();
			viewErrorMessage(); // Display last error message	
		} else if (! displayOff) {
			menu();
		}	
			
		// turn off display after DISPLAY_TIMEOUT
		if (displayOff && ((timeSince(userActivity) < DISPLAY_TIMEOUT))) {
			// wake up
			displayOff = 0;
			OLED_command(OLED_DISPLAYCONTROL | OLED_DISPLAYON);
		} else if (timeSince(userActivity) > DISPLAY_TIMEOUT) {
			displayOff = 1;
			OLED_command(OLED_DISPLAYCONTROL | OLED_DISPLAYOFF);
		} 		
		
		
		
		// Time to relax :-)
		__delay_ms(40);
		NOP();
	}
}

//-----------------------------------------------------------------------------
// Initialize global data structure
//-----------------------------------------------------------------------------

void initGlobalData(void)
{
	uint8_t n;
	t_heater *heater;

	data.tempC = 0;
	data.relHum = 0;
	data.dewPointC = 0;
	data.sensorVersion = 0;
	data.tempAux = 0;
	data.voltage = 0;
	data.current = 0;
	data.power = 0;
	data.dpOffset = 3.0;
	data.skyTemp = -40;
	data.fudgeFactor = 1.0;

	for (n = 0; n < NUM_CHANNELS; n++) {
		heater = &(data.heater)[n];
		heater->lensDia = 4;
		heater->status = CH_UNCHECKED;
		heater->mode = MODE_AUTO;
		heater->Pmax = 0;
		heater->Pset = -1;
		heater->Preq = 0;
		heater->Patt = 0;
		heater->current = 0;
	}
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
	RX1DTPPSbits.RX1DTPPS = 0x17; //RC7->EUSART1:RX1;
	RC6PPS = 0x0F; //RC4->EUSART1:TX1;    

	// Analog/digital IO
	ANSELA = 0b01000000; // RA6: aux. temp. sensor analog input
	ANSELB = 0b00000000; // PORTB is digital only
	ANSELC = 0b00000011; // RC0: VSENS, RC1: ISENS

	// Data direction registers
	TRISA = 0b11110000; // Outputs: RA0..3: SW_CH1..4, Inputs: RA4..7
	TRISB = 0b00000000;
	TRISC = 0b10000111; // Inputs: RC7: RX, RC2: nFAULT, RC1: ISENS, RC0: VSENS
	LATA = 0;

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
	INTCON = 0b11000000; // GIE, PEIE

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
