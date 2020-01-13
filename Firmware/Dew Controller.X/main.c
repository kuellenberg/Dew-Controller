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
    
    // Interrupts
    PIE0 = 0b00110000;      // TMR0IE, IOCIE
    PIE3 = 0b00100000;      // RC1IE
    PIE4 = 0b00000001;      // TMR1IE
    //INTCON = 0b11000000;    // GIE, PEIE
    
    // Interrupt-on-change
    IOCAP = 0b10110000;     // Pos. edge on RA7, RA5, RA4 (PB, ROT_B, ROT_A)
    IOCAN = 0b10110000;     // Neg. edge on RA7, RA5, RA4
    IOCCN = 0b00000100;     // Neg. edge on RC2 (nFAULT)

}

void __interrupt() isr(void)
{
    if(PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1)
    {
        // Timer 0 ISR
        TMR0 = 255-156;
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
        } 
        else if(PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            // EUSART RX ISR
        } 
    } 
}

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
