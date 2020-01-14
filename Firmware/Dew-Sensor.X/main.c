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
t_sensorData g_sensorData;
t_sensorVersion g_sensorVersion;


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void initialize(void);
void eusartTransmit(char *s);

//-----------------------------------------------------------------------------
// Main program loop
//-----------------------------------------------------------------------------

void main(void)
{
    char s[20];
    
    initialize();
    
    sprintf(s, "%.2f,%.2f,%.2f\n", 23.5, 44.3, -3.2);
 
    while (1)
    {
        CLRWDT();
        eusartTransmit(s);
        __delay_ms(500);
    }
}

void eusartTransmit(char *s)
{
    do
    {
        TX1REG = *s++;
        NOP();
        while(!PIR3bits.TX1IF);
    } while(*s != (char)NULL);
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

void initialize(void)
{
    OSCFRQ = 0b00000010;    // 4 MHz
    OSCCON1 = 0b01100000;   // HINTOSC (1-32MHz), CDIV = 1
    while(!OSCCON3bits.ORDY);   // Wait until clock switch is done
    
    // Peripheral Pin Select (PPS)
    RC4PPS = 0x0F;  //RC4->EUSART1:TX1;    
    
    // Digital I/O only
    ANSELA = 0;
    ANSELC = 0;

    // Data direction registers
    TRISA = 0;
    TRISC = 0b00100000;

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
    //INTCON = 0b11000000;    // GIE, PEIE

    // EUSART
    // SYNC = 0, BRGH = 0, BRG16 = 1, SPBRG = 25 -> 9615 Baud (0.16% error)
    BAUD1CON = 0b00001000; // BRG16 = 1
    SPBRGL = 25;
    RC1STA = 0b10010000; // SPEN = 1, CREN = 1
    TX1STA = 0b00100000; // TXEN = 1
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------

void __interrupt() ISR(void)
{
    /*
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
            PIR3bits.RC1IF = 0;
        }
    }
    */
}