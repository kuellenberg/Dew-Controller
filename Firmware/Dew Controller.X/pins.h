/* 
 * File:   pins.h
 * Author: Andre
 *
 * Created on 12. Januar 2020, 17:44
 */

#ifndef PINS_H
#define	PINS_H

#define SW_CH0      LATAbits.LATA0
#define SW_CH1      LATAbits.LATA1
#define SW_CH2      LATAbits.LATA2
#define SW_CH3      LATAbits.LATA3

#define ROT_A       PORTAbits.RA4
#define ROT_B       PORTAbits.RA5
#define ROT_PB      PORTAbits.RA7

#define OLED_DB3    LATCbits.LATC4
#define OLED_DB2    LATCbits.LATC5
#define OLED_DB1    LATBbits.LATB0
#define OLED_DB0    LATBbits.LATB1
#define OLED_EN     LATBbits.LATB2
#define OLED_RW     LATBbits.LATB3
#define OLED_RS     LATBbits.LATB4
#define OLED_PWR    LATBbits.LATB5

#define OLED_DB3_IN PORTCbits.RC4
#define OLED_DB3_T  TRISCbits.TRISC4

#define PEN         LATCbits.LATC3
#define nFAULT      PORTCbits.RC2

#define AIN_VSENS   0b010000
#define AIN_ISENS   0b010001
#define AIN_TEMP    0b000110

#endif	/* PINS_H */

