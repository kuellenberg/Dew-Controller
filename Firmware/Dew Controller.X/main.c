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

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TMR0_PRELOAD 178
#define TMR1_PRELOAD 53035

#define RX_BUF_LEN 20

typedef struct
{
	float tempC;
	float relHum;
	float dewPointC;
} t_sensorData;

typedef struct
{
	uint8_t hwMajor;
	uint8_t hwMinor;
	uint8_t swMajor;
	uint8_t swMinor;
} t_sensorVersion;


//-----------------------------------------------------------------------------
// Global Data
//-----------------------------------------------------------------------------
uint8_t g_rxFErrCount = 0;
uint8_t g_rxOErrCount = 0;
t_sensorData	g_sensorData;
t_sensorVersion g_sensorVersion;


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void initialize(void);
void EusartRxIsr(void);

//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------
void main(void)
{
    initialize();
    OLED_PWR = 1;
    PEN = 1;
    OLED_init();
    
    OLED_returnHome();
    OLED_command(OLED_CLEARDISPLAY);
    OLED_print_xy(0,0,"Hello World!");
    
    INTCON = 0b11000000;    // GIE, PEIE
    
    while(1)
    {
        NOP();
    }
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------
void initialize(void)
{
    // Analog/digital IO
    ANSELA = 0b0100000;     // RA6: aux. temp. sensor analog input
    ANSELB = 0b0000000;     // PORTB is digital only
    ANSELC = 0b0000011;     // RC0: VSENS, RC1: ISENS
    
    // Data direction registers
    TRISA = 0b11110000;     // Outputs: RA0..3: SW_CH1..4, Inputs: RA4..7
    TRISB = 0b00000000;
    TRISC = 0b10000111;     // Inputs: RC7: RX, RC2: nFAULT, RC1: ISENS, RC0: VSENS
    
    // ADC
    ADCON1 = 0b11100000;    // Right justified, Fosc/64, Vref+=Vdd

    // Timer0: 10ms 
    T0CON0 = 0b10000000;    // Enabled, 8-bit mode, postscaler 1:1
    T0CON1 = 0b01000111;    // Clock Fosc/4, sync to Fosc, prescaler 1:128
    TMR0 = TMR0_PRELOAD;
	
    // Timer1: 100ms
    T1CON = 0b00110011;     // Prescaler 1:8, 16-bit rd/wr, enabled
    T1CLK = 0b00000001;     // Clock Fosc/4
    TMR1 = TMR1_PRELOAD;
    
    // Interrupts
    PIE0 = 0b00110000;      // TMR0IE, IOCIE
    PIE3 = 0b00100000;      // RC1IE
    PIE4 = 0b00000001;      // TMR1IE
    //INTCON = 0b11000000;    // GIE, PEIE
    
    // Interrupt-on-change
    IOCAP = 0b10110000;     // Pos. edge on RA7, RA5, RA4 (PB, ROT_B, ROT_A)
    IOCAN = 0b10110000;     // Neg. edge on RA7, RA5, RA4
    IOCCN = 0b00000100;     // Neg. edge on RC2 (nFAULT)
	
	// EUSART
	// SYNC = 0, BRGH = 0, BRG16 = 1, SPBRG = 25 -> 9615 Baud (0.16% error)
	BAUD1CON = 0b00001000;	// BRG16 = 1
	SPBRGL = 25;
	RC1STA = 0b10010000;	// SPEN = 1, CREN = 1
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------
void __interrupt() ISR(void)
{
    if(PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1)
    {
        // Timer 0 ISR
        TMR0 = TMR0_PRELOAD;
        PIR0bits.TMR0IF = 0;        
        SW_CH1 = ~SW_CH1;
        NOP();
    }
    else if(PIE0bits.IOCIE == 1 && PIR0bits.IOCIF == 1)
    {
        // IOC ISR
    }
    else if(INTCONbits.PEIE == 1)
    {
        if(PIE4bits.TMR1IE == 1 && PIR4bits.TMR1IF == 1)
        {
            // Timer 1 ISR
			TMR1 = TMR1_PRELOAD;
			PIR4bits.TMR1IF = 0;
			SW_CH2 = ~SW_CH2;
			NOP();
        } 
        else if(PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            // EUSART RX ISR
			EusartRxIsr();
			PIR3bits.RC1IF = 0;
        } 
    } 
}

//-----------------------------------------------------------------------------
// EUSART Receive Interrupt
//-----------------------------------------------------------------------------
void EusartRxIsr(void)
{
	static char buffer[RX_BUF_LEN];
	static uint8_t rxCount = 0;
	
	if (RC1STAbits.OERR)
    {
        // Receiver buffer overrun error
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
		g_rxOErrCount++;
    }
	if (RC1STAbits.FERR)
	{
		RC1STAbits.SPEN = 0;
		RC1STAbits.SPEN = 1;
		g_rxFErrCount++;
	}
	
	if (rxCount < RX_BUF_LEN)
	{
		buffer[rxCount++] = RC1REG;
	} else 
	{
		rxCount = 0;
	}
}
