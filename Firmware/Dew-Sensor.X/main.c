/*
 * File:   main.c
 * Author: Andre
 *
 * Created on 12. Januar 2020, 13:20
 */

#define _XTAL_FREQ  4000000UL

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "config.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define B 17.67 // constants needed for dewpoint calculation
#define C 243.5

typedef struct {
	uint8_t header;
	uint8_t version;
	uint8_t status;
	float tempC;
	float relHum;
	float dewPointC;
} t_dataPacket;

typedef enum {
	NO_COMMAND,
	GET_DATA,
	UNKNOWN_COMMAND
} t_commands;

//-----------------------------------------------------------------------------
// Global Data
//-----------------------------------------------------------------------------

volatile uint8_t g_rxFErrCount = 0;
volatile uint8_t g_rxOErrCount = 0;
volatile t_commands g_command;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void initialize(void);
void uartSendByte(char s);
void uartReceiveISR(void);
void i2cStart(void);
void i2cStop(void);
void i2cRepeat(void);
uint8_t i2cRead(uint8_t *readBuffer, uint8_t numBytes, uint8_t address);
uint8_t i2cWrite(uint8_t *writeBuffer, uint8_t numBytes, uint8_t address);
uint8_t readSI7006(float *RH, float *tempC, float *DP);

//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------

void main(void)
{
	char *s;
	char checksum;
	t_dataPacket dataPacket;
	uint8_t n;

	initialize();

	while (1) {
		CLRWDT();
		switch (g_command) {
		case GET_DATA:
			dataPacket.header = 0xAA;
			dataPacket.version = 10;

			dataPacket.status = 0;
			if (readSI7006(&dataPacket.relHum, &dataPacket.tempC, &dataPacket.dewPointC)) {
				if ((dataPacket.tempC >= -50) && (dataPacket.tempC <= 100) &&
					(dataPacket.dewPointC >= -50) && (dataPacket.dewPointC <= 100) &&
					(dataPacket.relHum >= 0) && (dataPacket.relHum <= 100))
					dataPacket.status = 1;
			}

			s = (char *) &dataPacket;
			checksum = 0;
			for (n = 0; n < sizeof(t_dataPacket); n++) {
				checksum ^= *s;
				uartSendByte(*(s++));
			}
			uartSendByte(checksum);

			g_command = NO_COMMAND;
			break;
		case UNKNOWN_COMMAND:
			uartSendByte(0xFF);
			g_command = NO_COMMAND;
			break;
		default:
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Read humidity and temperature values from SI7006 sensor
//-----------------------------------------------------------------------------

uint8_t readSI7006(float *RH, float *tempC, float *DP)
{
	uint8_t buffer[2];
	float gamma;

	i2cStart();
	buffer[0] = 0xE5; // measure relative humidity, hold master mode
	if (!i2cWrite(buffer, 1, 0x40))
		return 0;
	i2cRepeat();
	if (!i2cRead(buffer, 2, 0x40))
		return 0;
	i2cStop();

	*RH = (125.0 * ((uint16_t) buffer[0] << 8 | (uint16_t) buffer[1]) / 65536) - 6;

	i2cStart();
	buffer[0] = 0xE0; // get temperature from last humidity measurement
	if (!i2cWrite(buffer, 1, 0x40))
		return 0;
	i2cRepeat();
	if (!i2cRead(buffer, 2, 0x40))
		return 0;
	i2cStop();

	*tempC = (175.72 * ((uint16_t) buffer[0] << 8 | (uint16_t) buffer[1]) / 65536) - 46.85;

	gamma = log(*RH / 100.0) + ((B * *tempC) / (C + *tempC));
	*DP = (C * gamma) / (B - gamma);

	return 1;
}

//-----------------------------------------------------------------------------
// Generate I2C start condition
//-----------------------------------------------------------------------------

void i2cStart(void)
{
	PIR3bits.SSP1IF = 0;
	SSP1CON2bits.SEN = 1;
	while (!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
}

//-----------------------------------------------------------------------------
// Generate I2C stop condition
//-----------------------------------------------------------------------------

void i2cStop(void)
{
	SSP1CON2bits.PEN = 1;
	while (!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
}

//-----------------------------------------------------------------------------
// Generate I2C repeat condition
//-----------------------------------------------------------------------------

void i2cRepeat(void)
{
	SSP1CON2bits.RSEN = 1;
	while (!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
}

//-----------------------------------------------------------------------------
// Writes numBytes from writeBuffer to I2C device
//-----------------------------------------------------------------------------

uint8_t i2cWrite(uint8_t *writeBuffer, uint8_t numBytes, uint8_t address)
{
	uint8_t n;

	SSP1BUF = address << 1;
	while (!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
	if (SSP1CON2bits.ACKSTAT)
		return 0;
	for (n = 0; n < numBytes; n++) {
		SSP1BUF = writeBuffer[n];
		while (!PIR3bits.SSP1IF);
		PIR3bits.SSP1IF = 0;
		if (SSP1CON2bits.ACKSTAT)
			return n;
	}
	return n;
}

//-----------------------------------------------------------------------------
// Read numBytes from I2C device into readBuffer
//-----------------------------------------------------------------------------

uint8_t i2cRead(uint8_t *readBuffer, uint8_t numBytes, uint8_t address)
{
	SSP1BUF = (address << 1) | 0x01;
	while (!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
	if (SSP1CON2bits.ACKSTAT)
		return 0;
	while (numBytes--) {
		SSP1CON2bits.RCEN = 1;
		while (!PIR3bits.SSP1IF);
		PIR3bits.SSP1IF = 0;
		*readBuffer++ = SSP1BUF;

		SSP1CON2bits.ACKDT = (numBytes ? 0 : 1);
		SSP1CON2bits.ACKEN = 1;
		while (!PIR3bits.SSP1IF);
		PIR3bits.SSP1IF = 0;
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Transmit single byte over UART
//-----------------------------------------------------------------------------

void uartSendByte(char s)
{
	TX1REG = s;
	NOP();
	while (!PIR3bits.TX1IF);
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

void initialize(void)
{
	// Configure oscillator
	OSCFRQ = 0b00000010; // 4 MHz
	OSCCON1 = 0b01100000; // HINTOSC (1-32MHz), CDIV = 1
	while (!OSCCON3bits.ORDY); // Wait until clock switch is done

	// Peripheral Pin Select (PPS)
	RC4PPS = 0x0F; //RC4->EUSART1:TX1;
	RC1PPS = 0x16; // RC1->SDA (output)
	SSP1DATPPS = 0x11; // RC1->SDA (input)
	RC0PPS = 0x15; // RC0->SCL (output)

	// Configure ports for digital I/O
	ANSELA = 0;
	ANSELC = 0;

	// Data direction registers
	TRISA = 0;
	TRISC = 0b00100011; // I2C pins must be configured as inputs in master mode

	// Interrupts
	PIE3 = 0b00100000; // RC1IE
	INTCON = 0b11000000; // GIE, PEIE

	// EUSART
	// SYNC = 0, BRGH = 0, BRG16 = 1, SPBRG = 25 -> 9615 Baud (0.16% error)
	BAUD1CON = 0b00001000; // BRG16 = 1
	SPBRGL = 25;
	RC1STA = 0b10010000; // SPEN = 1, CREN = 1
	TX1STA = 0b00100000; // TXEN = 1

	// I2C
	SSP1ADD = 0x09; // 100kHz Fclock = Fosc / ((SSP1ADD + 1) * 4) 
	SSP1CON1 = 0b00101000; // SSPEN = 1, I2C Master mode	
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------

void __interrupt() ISR(void)
{
	if (INTCONbits.PEIE == 1 && PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1) {
		uartReceiveISR();
		PIR3bits.RC1IF = 0;
	}
}

//-----------------------------------------------------------------------------
// EUSART Receive Interrupt
//-----------------------------------------------------------------------------

void uartReceiveISR(void)
{
	char rxByte;

	if (RC1STAbits.OERR) { // Receiver buffer overrun error
		RC1STAbits.CREN = 0;
		RC1STAbits.CREN = 1;
		g_rxOErrCount++;
	}
	if (RC1STAbits.FERR) { // Framing error
		RC1STAbits.SPEN = 0;
		RC1STAbits.SPEN = 1;
		g_rxFErrCount++;
	}

	rxByte = RC1REG;
	if (rxByte == '?')
		g_command = GET_DATA;
	else
		g_command = UNKNOWN_COMMAND;
}
