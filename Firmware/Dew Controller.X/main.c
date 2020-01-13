/*
 * File:   main.c
 * Author: Andre
 *
 * Created on 12. Januar 2020, 13:20
 */


#include <xc.h>
#include "config.h"
#include "pins.h"
#include "oled.h"

void initialize()
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
    T0CON1 = 0b01001000;    // Clock Fosc/4, sync to Fosc, prescaler 1:256
    TMR0 = 156;
    // Timer1: 100ms
    T1CON = 0b00110011;     // Prescaler 1:8, 16-bit rd/wr, enabled
    T1CLK = 0b00000001;     // Clock Fosc/4
    TMR1 = 50000;
    
    //Interrupts
    
}


void main(void)
{
    return;
}
