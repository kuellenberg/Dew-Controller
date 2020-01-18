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
#include "config.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TMR0_PRELOAD 178
#define TMR1_PRELOAD 53035

#define RX_BUF_LEN 20

typedef struct
{
    uint8_t header;
    uint8_t version;
    float tempC;
    float relHum;
    float dewPointC;
} t_dataPacket;

typedef enum
{
    NO_COMMAND,
    GET_DATA,
    UNKNOWN_COMMAND
} t_commands;

//-----------------------------------------------------------------------------
// Global Data
//-----------------------------------------------------------------------------

uint8_t g_rxFErrCount = 0;
uint8_t g_rxOErrCount = 0;
t_dataPacket g_dataPacket;
t_commands g_command;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void initialize(void);
void eusartTransmit(char *s);
void eusartRxIsr(void);
void i2cStart(void);
void i2cStop(void);
void i2cRepeat(void);
uint8_t i2cRead(uint8_t *readBuffer, uint8_t numBytes, uint8_t address);
uint8_t i2cWrite(uint8_t *writeBuffer, uint8_t numBytes, uint8_t address);

//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------

void main(void)
{
    char *s;
    char checksum;
    uint8_t n, len;
	uint8_t writeBuffer;
	uint8_t readBuffer[2];

    initialize();

    g_dataPacket.header = 0xAA;
    g_dataPacket.version = 0x01;
    g_dataPacket.tempC = 8.6;
    g_dataPacket.relHum = 86.6;
    g_dataPacket.dewPointC = 7.2;
	
	
	i2cStart();
	writeBuffer = 0xE5;	// measure relative humidity, hold master mode
	i2cWrite(writeBuffer, 1, 0x40);
	i2cRepeat();
	i2cRead(readBuffer, 2, 0x40);
	i2cStop();
	
	NOP();
	

    while (1)
    {
        CLRWDT();
        switch (g_command)
        {
        case GET_DATA:
            g_command = NO_COMMAND;
            s = (char *) &g_dataPacket;
            checksum = 0;
            for (n = 0; n < sizeof (t_dataPacket); n++)
            {
                checksum ^= *s;
                TX1REG = *(s++);
                NOP();
                while (!PIR3bits.TX1IF);
            }
            TX1REG = checksum;
            NOP();
            while (!PIR3bits.TX1IF);
            break;
        case UNKNOWN_COMMAND:
            g_command = NO_COMMAND;
            TX1REG = 0xFF;
            NOP();
            while (!PIR3bits.TX1IF);
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Generate I2C start condition
//-----------------------------------------------------------------------------

void i2cStart(void)
{
	PIR3bits.SSP1IF = 0;
	SSP1CON2bits.SEN = 1;
	while(!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
}

//-----------------------------------------------------------------------------
// Generate I2C stop condition
//-----------------------------------------------------------------------------

void i2cStop(void)
{
	SSP1CON2bits.PEN = 1;
	while(!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
}

//-----------------------------------------------------------------------------
// Generate I2C repeat condition
//-----------------------------------------------------------------------------

void i2cRepeat(void)
{
	SSP1CON2bits.RSEN = 1;
	while(!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
}

//-----------------------------------------------------------------------------
// Writes numBytes from writeBuffer to I2C device
//-----------------------------------------------------------------------------

uint8_t i2cWrite(uint8_t *writeBuffer, uint8_t numBytes, uint8_t address)
{
	uint8_t n;
	
	SSP1BUF = address;
	while(!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
	if (SSP1CON2bits.ACKSTAT) 
		return 0;
	for(n = 0; n < numBytes; n++) {
		SSP1BUF = writeBuffer[n];
		while(!PIR3bits.SSP1IF);
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
	SSP1BUF = address | 0x80;
	while(!PIR3bits.SSP1IF);
	PIR3bits.SSP1IF = 0;
	if (SSP1CON2bits.ACKSTAT) 
		return 0;
	while(numBytes--)
	{
		SSP1CON2bits.RCEN = 1;
		while(!PIR3bits.SSP1IF);
		PIR3bits.SSP1IF = 0;
		*readBuffer++ = SSP1BUF;
	
		SSP1CON2bits.ACKDT = (numBytes ? 0 : 1);
		SSP1CON2bits.ACKEN = 1;
		while(!PIR3bits.SSP1IF);
		PIR3bits.SSP1IF = 0;
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Transmit character string over UART
//-----------------------------------------------------------------------------

void eusartTransmit(char *s)
{
    do
    {
        TX1REG = *s++;
        NOP();
        while (!PIR3bits.TX1IF);
    }
    while (*s != (char) NULL);
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

void initialize(void)
{
    OSCFRQ = 0b00000010; 		// 4 MHz
    OSCCON1 = 0b01100000;		// HINTOSC (1-32MHz), CDIV = 1
    while (!OSCCON3bits.ORDY);	// Wait until clock switch is done

    // Peripheral Pin Select (PPS)
    RC4PPS = 0x0F; 		//RC4->EUSART1:TX1;
	RC1PPS = 0x16;		// RC1->SDA (output)
	SSPDATPPS = 0x11;	// RC1->SDA (input)
	RC0PPS = 0x15;		// RC0->SCL (output)

    // Digital I/O only
    ANSELA = 0;
    ANSELC = 0;

    // Data direction registers
    TRISA = 0;
    TRISC = 0b00100011;	// I2C pins must be configured as inputs in master mode

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

    // EUSART
    // SYNC = 0, BRGH = 0, BRG16 = 1, SPBRG = 25 -> 9615 Baud (0.16% error)
    BAUD1CON = 0b00001000; // BRG16 = 1
    SPBRGL = 25;
    RC1STA = 0b10010000; // SPEN = 1, CREN = 1
    TX1STA = 0b00100000; // TXEN = 1

    // I2C
	SSP1ADD = 0x09; // 100kHz Fclock = Fosc / ((SSP1ADD + 1) * 4) 
	SSP1CON1 = 0b00101000;	// SSPEN = 1, I2C Master mode	
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------

void __interrupt() ISR(void)
{
    if (PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1)
    {
        // Timer 0 ISR
        TMR0 = TMR0_PRELOAD;
        PIR0bits.TMR0IF = 0;
    }
    else if (PIE0bits.IOCIE == 1 && PIR0bits.IOCIF == 1)
    {
        // IOC ISR
    }
    else if (INTCONbits.PEIE == 1)
    {
        if (PIE4bits.TMR1IE == 1 && PIR4bits.TMR1IF == 1)
        {
            // Timer 1 ISR
            TMR1 = TMR1_PRELOAD;
            PIR4bits.TMR1IF = 0;
        }
        else if (PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            // EUSART RX ISR
            eusartRxIsr();
            PIR3bits.RC1IF = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// EUSART Receive Interrupt
//-----------------------------------------------------------------------------

void eusartRxIsr(void)
{
    char rx;

    if (RC1STAbits.OERR)
    { // Receiver buffer overrun error
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
        g_rxOErrCount++;
    }
    if (RC1STAbits.FERR)
    { // Framing error
        RC1STAbits.SPEN = 0;
        RC1STAbits.SPEN = 1;
        g_rxFErrCount++;
    }

    rx = RC1REG;
    if (rx == '?')
        g_command = GET_DATA;
    else
        g_command = UNKNOWN_COMMAND;
}
