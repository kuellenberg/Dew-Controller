/*
 * File:   main.c
 * Author: Andre
 *
 * Created on 12. Januar 2020, 13:20
 */

#include "common.h"
#include "config.h"
#include "oled.h"

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TMR0_PRELOAD 178
#define TMR1_PRELOAD 53035
#define SENSOR_TIMER 5
#define RX_BUF_LEN 20
#define COLUMNS 12
#define ADC_ARRAY_SIZE 10
#define ADC_CHANNELS 3

typedef struct {
	uint8_t header;
	uint8_t version;
	uint8_t status;
	float tempC;
	float relHum;
	float dewPointC;
} t_dataPacket;

enum e_states {START = 0, CW1, CW2, CW3, CCW1, CCW2, CCW3};
enum e_flags {CW_FLAG = 0b10000000, CCW_FLAG = 0b01000000};
enum e_direction {ROT_STOP, ROT_CW, ROT_CCW};
enum e_buttonPress {PB_NONE, PB_SHORT, PB_LONG, PB_ABORT};

const uint8_t g_adcChannels[ADC_CHANNELS] = {AIN_TEMP, AIN_VSENS, AIN_ISENS};
volatile uint8_t g_adcArrayIdx = 0;
volatile uint16_t g_adcArray[ADC_ARRAY_SIZE];
volatile uint16_t g_adcArrayAvg;
volatile uint32_t g_adcArraySum;

//-----------------------------------------------------------------------------
// Global Data
//-----------------------------------------------------------------------------

// transition table for encoder FSM 
const uint8_t transition_table[7][4] = {
/*                 00           01      10      11  */
/* -----------------------------------------------------*/
/*START     | */  {START,       CCW1,   CW1,    START},
/*CW_Step1  | */  {CW2|CW_FLAG, START,  CW1,    START},
/*CW_Step2  | */  {CW2,         CW3,    CW1,    START},
/*CW_Step3  | */  {CW2,         CW3,    START,  START|CW_FLAG},
/*CCW_Step1 | */  {CCW2|CCW_FLAG,CCW1,  START,  START},
/*CCW_Step2 | */  {CCW2,        CCW1,   CCW3,   START},
/*CCW_Step3 | */  {CCW2,        START,  CCW3,   START|CCW_FLAG}
};

volatile uint8_t g_curRotState = START;
volatile enum e_direction g_rotDir = ROT_STOP;
volatile enum e_buttonPress g_pbState = PB_NONE;

volatile uint8_t g_10msTick = 0;
volatile uint8_t g_100msTick = 0;
volatile uint8_t g_sensorTimer  = 0;
volatile uint8_t g_rxFErrCount = 0;
volatile uint8_t g_rxOErrCount = 0;
volatile uint8_t g_dataReadyFlag = 0;
volatile t_dataPacket g_dataPacket;

float g_tempC, g_relHum, g_dewPointC, g_sensorVersion;
float g_voltage, g_current, g_power;


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void initialize(void);
void uartReceiveISR(void);
void pushButtonISR(void);
void rotISR(void);
void uartSendByte(char s);
void showMenu(void);
void menuInput(uint8_t *page, const uint8_t numPages, uint8_t *menu, 
	uint8_t pbShort, uint8_t pbLong, uint8_t timeout);
enum e_direction getRotDir(void);
enum e_buttonPress getPB(void);
void handleSensorData(void);
void readAnalogValues(void);

//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------

void main(void)
{
	char s1[16], s2[16];

	initialize();
	OLED_PWR = 1;
	PEN = 1;
	OLED_init();
	OLED_returnHome();
	OLED_command(OLED_CLEARDISPLAY);
	
	
	SW_CH1 = 1;

	while (1) {
		CLRWDT();
		
		if (g_adcReadyFlag == 1) {
			g_adcReadyFlag = 0;
			getAnalogValues();
		}
		
		if (g_sensorTimer >= SENSOR_TIMER) {
			g_sensorTimer = 0;
			uartSendByte('?');
		}
		
		if (g_dataReadyFlag == 1) {
			g_dataReadyFlag = 0;
			handleSensorData();
		}
		
		showMenu();
		__delay_ms(10);
	
	}
}

void getAnalogValues(void)
{
	g_tempAux = g_adcArrayAvg[0] * 0.1191 - 34.512;
	g_voltage = (g_adcArrayAvg[1] * 5.0 * (150.0 + 47.0)) / (1023.0 * 47.0);
	g_current = (g_adcArrayAvg[2] * 5.0) / (1023.0 * 0.05 * 50.0);
	g_power = g_voltage * g_current;
}

void handleSensorData(void)
{
	if ((g_dataPacket.header == 0xAA) && (g_dataPacket.status == 1)) {
		g_tempC = g_dataPacket.tempC;
		g_relHum = g_dataPacket.relHum;
		g_dewPointC = g_dataPacket.dewPointC;
		g_sensorVersion = g_dataPacket.version;
	} else {
		// set error bits
	}
}

void showMenu(void)
{
	static uint8_t menu = 0;
	static uint8_t page = 0;
	enum e_buttonPress pb;
	char s[61];

	pb = getPB();
	if (menu == 0) {
		// Main menu
		OLED_print_xy(0, 0, "Temperature Rel.humidityDewpoint    Bat.   Power");
		sprintf(s, "%5.1f \xdf\C    %5.1f %%     %5.1f \xdf\C    %4.1fV  %4.1fW", 
			g_tempC, g_relHum, g_dewPointC, g_voltage, g_power);
		OLED_print_xy(0, 1, s);
		menuInput(&page, 4, &menu, 1, 0, 0);
	} else if (menu == 1) {
		// Display power for each channel
		OLED_print_xy(0, 0, "Ch1: xx inchCh2: xx inchCh3: xx inchCh4: xx inch");
		menuInput(&page, 4, &menu, 1, 0, 0);
	}
}

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

enum e_direction getRotDir(void)
{
	enum e_direction ret = g_rotDir;
	g_rotDir = ROT_STOP;
	return ret;
}

enum e_buttonPress getPB(void)
{
	enum e_buttonPress ret = g_pbState;
	g_pbState = PB_NONE;
	return ret;
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
	PIE1 = 0b00000001; // ADIE
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

//-----------------------------------------------------------------------------
// Transmit character string over UART
//-----------------------------------------------------------------------------

void uartSendByte(char s)
{
	TX1REG = s;
	NOP();
	while (!PIR3bits.TX1IF);
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------

void __interrupt() ISR(void)
{
	if (PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1) {
		// Timer 0 ISR
		// TODO: 1 or 10ms tick for pushbutton?
		g_10msTick++;
		TMR0 = TMR0_PRELOAD;
		PIR0bits.TMR0IF = 0;
	} else if (PIE0bits.IOCIE == 1 && PIR0bits.IOCIF == 1) {
		// Interrupt on change ISRs
		if (IOCAFbits.IOCAF7 == 1) {
			pushButtonISR();
			IOCAFbits.IOCAF7 = 0;
		}
		if (IOCAFbits.IOCAF4 == 1) {
			rotISR();
			IOCAFbits.IOCAF4 = 0;
		}
		if (IOCAFbits.IOCAF5 == 1) {
			rotISR();
			IOCAFbits.IOCAF5 = 0;
		}
		PIR0bits.IOCIF = 0;
	} else if (INTCONbits.PEIE == 1) {
		if (PIE4bits.TMR1IE == 1 && PIR4bits.TMR1IF == 1) {
			// Timer 1 ISR
			if (++g_100msTick == 10) {
				g_100msTick = 0;
				g_sensorTimer++;
			}			
			TMR1 = TMR1_PRELOAD;
			PIR4bits.TMR1IF = 0;

		} else if (PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1) {
			uartReceiveISR();
			PIR3bits.RC1IF = 0;
		} else if (PIE1bits.ADIE == 1 && PIR1bits.ADIF == 1) {
			adcISR();
			PIR1bits.ADIF = 0;
		}
	}
}

void adcISR(void)
{
	g_adcArraySum -= g_adcArraySum;
	g_adcArray[g_adcArrayIdx] = (uint16_t)((ADRESH << 8) + ADRESL);
	g_adcArraySum += g_adcArray[g_adcArrayIdx];
	g_adcArrayAvg = (uint16_t)(g_adcArraySum / ADC_ARRAY_SIZE);
	if (++g_adcArrayIdx >= ADC_ARRAY_SIZE) {
		g_adcArrayIdx = 0;
		g_adcReadyFlag = 1;
	}
}

//-----------------------------------------------------------------------------
// EUSART Receive Interrupt
//-----------------------------------------------------------------------------

void uartReceiveISR(void)
{
	static char buffer[RX_BUF_LEN];
	static uint8_t rxCount = 0;
	static uint8_t checksum = 0;

	if (RC1STAbits.OERR) // Receiver buffer overrun error
	{
		RC1STAbits.CREN = 0;
		RC1STAbits.CREN = 1;
		g_rxOErrCount++;
	}
	if (RC1STAbits.FERR) // Framing error
	{
		RC1STAbits.SPEN = 0;
		RC1STAbits.SPEN = 1;
		g_rxFErrCount++;
	}

	if (rxCount < sizeof(g_dataPacket)) {
		buffer[rxCount] = RC1REG;
		checksum ^= buffer[rxCount];
		rxCount++;
	} else {
		if (RC1REG == checksum) {
			g_dataReady = 1;
			strncpy((char *) &g_dataPacket, buffer, sizeof(g_dataPacket));
		}
		checksum = 0;
		rxCount = 0;
	}
}

void rotISR()
{
	uint8_t input;

	input = (ROT_B << 1) | ROT_A; // input code for rot_a and rot_b combined

	// set current state according to transition table (cw/ccw flags masked out)
	g_curRotState = transition_table[g_curRotState & 0b00000111][input];

	// set global direction flag
	if (g_curRotState & CW_FLAG) g_rotDir = ROT_CW;
	if (g_curRotState & CCW_FLAG) g_rotDir = ROT_CCW;
	NOP();
}

void pushButtonISR()
{
	// reset millisecond tick counter upon first rising edge of push button 
	if (!ROT_PB) {
		g_10msTick = 0;
	} else {
		if ((g_10msTick > 5) & (g_10msTick <= 50)) 
			// short button press
			g_pbState = PB_SHORT;		
		else if ((g_10msTick > 50) & (g_10msTick <= 150))
			// long button press
			g_pbState = PB_LONG; 
		else 
			// button pressed too long => abort
			g_pbState = PB_ABORT;
	}
}
