/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F15356
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45
        MPLAB 	          :  MPLAB X 4.15	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set SW_CH1 aliases
#define SW_CH1_TRIS                 TRISAbits.TRISA0
#define SW_CH1_LAT                  LATAbits.LATA0
#define SW_CH1_PORT                 PORTAbits.RA0
#define SW_CH1_WPU                  WPUAbits.WPUA0
#define SW_CH1_OD                   ODCONAbits.ODCA0
#define SW_CH1_ANS                  ANSELAbits.ANSA0
#define SW_CH1_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define SW_CH1_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define SW_CH1_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define SW_CH1_GetValue()           PORTAbits.RA0
#define SW_CH1_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define SW_CH1_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define SW_CH1_SetPullup()          do { WPUAbits.WPUA0 = 1; } while(0)
#define SW_CH1_ResetPullup()        do { WPUAbits.WPUA0 = 0; } while(0)
#define SW_CH1_SetPushPull()        do { ODCONAbits.ODCA0 = 0; } while(0)
#define SW_CH1_SetOpenDrain()       do { ODCONAbits.ODCA0 = 1; } while(0)
#define SW_CH1_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define SW_CH1_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set SW_CH2 aliases
#define SW_CH2_TRIS                 TRISAbits.TRISA1
#define SW_CH2_LAT                  LATAbits.LATA1
#define SW_CH2_PORT                 PORTAbits.RA1
#define SW_CH2_WPU                  WPUAbits.WPUA1
#define SW_CH2_OD                   ODCONAbits.ODCA1
#define SW_CH2_ANS                  ANSELAbits.ANSA1
#define SW_CH2_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define SW_CH2_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define SW_CH2_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define SW_CH2_GetValue()           PORTAbits.RA1
#define SW_CH2_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define SW_CH2_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define SW_CH2_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define SW_CH2_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define SW_CH2_SetPushPull()        do { ODCONAbits.ODCA1 = 0; } while(0)
#define SW_CH2_SetOpenDrain()       do { ODCONAbits.ODCA1 = 1; } while(0)
#define SW_CH2_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define SW_CH2_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set SW_CH3 aliases
#define SW_CH3_TRIS                 TRISAbits.TRISA2
#define SW_CH3_LAT                  LATAbits.LATA2
#define SW_CH3_PORT                 PORTAbits.RA2
#define SW_CH3_WPU                  WPUAbits.WPUA2
#define SW_CH3_OD                   ODCONAbits.ODCA2
#define SW_CH3_ANS                  ANSELAbits.ANSA2
#define SW_CH3_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define SW_CH3_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define SW_CH3_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define SW_CH3_GetValue()           PORTAbits.RA2
#define SW_CH3_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define SW_CH3_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define SW_CH3_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define SW_CH3_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define SW_CH3_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define SW_CH3_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define SW_CH3_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define SW_CH3_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set SW_CH4 aliases
#define SW_CH4_TRIS                 TRISAbits.TRISA3
#define SW_CH4_LAT                  LATAbits.LATA3
#define SW_CH4_PORT                 PORTAbits.RA3
#define SW_CH4_WPU                  WPUAbits.WPUA3
#define SW_CH4_OD                   ODCONAbits.ODCA3
#define SW_CH4_ANS                  ANSELAbits.ANSA3
#define SW_CH4_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define SW_CH4_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define SW_CH4_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define SW_CH4_GetValue()           PORTAbits.RA3
#define SW_CH4_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define SW_CH4_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define SW_CH4_SetPullup()          do { WPUAbits.WPUA3 = 1; } while(0)
#define SW_CH4_ResetPullup()        do { WPUAbits.WPUA3 = 0; } while(0)
#define SW_CH4_SetPushPull()        do { ODCONAbits.ODCA3 = 0; } while(0)
#define SW_CH4_SetOpenDrain()       do { ODCONAbits.ODCA3 = 1; } while(0)
#define SW_CH4_SetAnalogMode()      do { ANSELAbits.ANSA3 = 1; } while(0)
#define SW_CH4_SetDigitalMode()     do { ANSELAbits.ANSA3 = 0; } while(0)

// get/set ROT_A aliases
#define ROT_A_TRIS                 TRISAbits.TRISA4
#define ROT_A_LAT                  LATAbits.LATA4
#define ROT_A_PORT                 PORTAbits.RA4
#define ROT_A_WPU                  WPUAbits.WPUA4
#define ROT_A_OD                   ODCONAbits.ODCA4
#define ROT_A_ANS                  ANSELAbits.ANSA4
#define ROT_A_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define ROT_A_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define ROT_A_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define ROT_A_GetValue()           PORTAbits.RA4
#define ROT_A_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define ROT_A_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define ROT_A_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define ROT_A_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define ROT_A_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define ROT_A_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define ROT_A_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define ROT_A_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set ROT_B aliases
#define ROT_B_TRIS                 TRISAbits.TRISA5
#define ROT_B_LAT                  LATAbits.LATA5
#define ROT_B_PORT                 PORTAbits.RA5
#define ROT_B_WPU                  WPUAbits.WPUA5
#define ROT_B_OD                   ODCONAbits.ODCA5
#define ROT_B_ANS                  ANSELAbits.ANSA5
#define ROT_B_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define ROT_B_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define ROT_B_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define ROT_B_GetValue()           PORTAbits.RA5
#define ROT_B_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define ROT_B_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define ROT_B_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define ROT_B_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define ROT_B_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define ROT_B_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define ROT_B_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define ROT_B_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)

// get/set TEMP aliases
#define TEMP_TRIS                 TRISAbits.TRISA6
#define TEMP_LAT                  LATAbits.LATA6
#define TEMP_PORT                 PORTAbits.RA6
#define TEMP_WPU                  WPUAbits.WPUA6
#define TEMP_OD                   ODCONAbits.ODCA6
#define TEMP_ANS                  ANSELAbits.ANSA6
#define TEMP_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define TEMP_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define TEMP_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define TEMP_GetValue()           PORTAbits.RA6
#define TEMP_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define TEMP_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)
#define TEMP_SetPullup()          do { WPUAbits.WPUA6 = 1; } while(0)
#define TEMP_ResetPullup()        do { WPUAbits.WPUA6 = 0; } while(0)
#define TEMP_SetPushPull()        do { ODCONAbits.ODCA6 = 0; } while(0)
#define TEMP_SetOpenDrain()       do { ODCONAbits.ODCA6 = 1; } while(0)
#define TEMP_SetAnalogMode()      do { ANSELAbits.ANSA6 = 1; } while(0)
#define TEMP_SetDigitalMode()     do { ANSELAbits.ANSA6 = 0; } while(0)

// get/set ROT_PB aliases
#define ROT_PB_TRIS                 TRISAbits.TRISA7
#define ROT_PB_LAT                  LATAbits.LATA7
#define ROT_PB_PORT                 PORTAbits.RA7
#define ROT_PB_WPU                  WPUAbits.WPUA7
#define ROT_PB_OD                   ODCONAbits.ODCA7
#define ROT_PB_ANS                  ANSELAbits.ANSA7
#define ROT_PB_SetHigh()            do { LATAbits.LATA7 = 1; } while(0)
#define ROT_PB_SetLow()             do { LATAbits.LATA7 = 0; } while(0)
#define ROT_PB_Toggle()             do { LATAbits.LATA7 = ~LATAbits.LATA7; } while(0)
#define ROT_PB_GetValue()           PORTAbits.RA7
#define ROT_PB_SetDigitalInput()    do { TRISAbits.TRISA7 = 1; } while(0)
#define ROT_PB_SetDigitalOutput()   do { TRISAbits.TRISA7 = 0; } while(0)
#define ROT_PB_SetPullup()          do { WPUAbits.WPUA7 = 1; } while(0)
#define ROT_PB_ResetPullup()        do { WPUAbits.WPUA7 = 0; } while(0)
#define ROT_PB_SetPushPull()        do { ODCONAbits.ODCA7 = 0; } while(0)
#define ROT_PB_SetOpenDrain()       do { ODCONAbits.ODCA7 = 1; } while(0)
#define ROT_PB_SetAnalogMode()      do { ANSELAbits.ANSA7 = 1; } while(0)
#define ROT_PB_SetDigitalMode()     do { ANSELAbits.ANSA7 = 0; } while(0)

// get/set LCD_DB1 aliases
#define LCD_DB1_TRIS                 TRISBbits.TRISB0
#define LCD_DB1_LAT                  LATBbits.LATB0
#define LCD_DB1_PORT                 PORTBbits.RB0
#define LCD_DB1_WPU                  WPUBbits.WPUB0
#define LCD_DB1_OD                   ODCONBbits.ODCB0
#define LCD_DB1_ANS                  ANSELBbits.ANSB0
#define LCD_DB1_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define LCD_DB1_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define LCD_DB1_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define LCD_DB1_GetValue()           PORTBbits.RB0
#define LCD_DB1_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define LCD_DB1_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define LCD_DB1_SetPullup()          do { WPUBbits.WPUB0 = 1; } while(0)
#define LCD_DB1_ResetPullup()        do { WPUBbits.WPUB0 = 0; } while(0)
#define LCD_DB1_SetPushPull()        do { ODCONBbits.ODCB0 = 0; } while(0)
#define LCD_DB1_SetOpenDrain()       do { ODCONBbits.ODCB0 = 1; } while(0)
#define LCD_DB1_SetAnalogMode()      do { ANSELBbits.ANSB0 = 1; } while(0)
#define LCD_DB1_SetDigitalMode()     do { ANSELBbits.ANSB0 = 0; } while(0)

// get/set LCD_DB0 aliases
#define LCD_DB0_TRIS                 TRISBbits.TRISB1
#define LCD_DB0_LAT                  LATBbits.LATB1
#define LCD_DB0_PORT                 PORTBbits.RB1
#define LCD_DB0_WPU                  WPUBbits.WPUB1
#define LCD_DB0_OD                   ODCONBbits.ODCB1
#define LCD_DB0_ANS                  ANSELBbits.ANSB1
#define LCD_DB0_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define LCD_DB0_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define LCD_DB0_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define LCD_DB0_GetValue()           PORTBbits.RB1
#define LCD_DB0_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define LCD_DB0_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define LCD_DB0_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define LCD_DB0_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define LCD_DB0_SetPushPull()        do { ODCONBbits.ODCB1 = 0; } while(0)
#define LCD_DB0_SetOpenDrain()       do { ODCONBbits.ODCB1 = 1; } while(0)
#define LCD_DB0_SetAnalogMode()      do { ANSELBbits.ANSB1 = 1; } while(0)
#define LCD_DB0_SetDigitalMode()     do { ANSELBbits.ANSB1 = 0; } while(0)

// get/set LCD_EN aliases
#define LCD_EN_TRIS                 TRISBbits.TRISB2
#define LCD_EN_LAT                  LATBbits.LATB2
#define LCD_EN_PORT                 PORTBbits.RB2
#define LCD_EN_WPU                  WPUBbits.WPUB2
#define LCD_EN_OD                   ODCONBbits.ODCB2
#define LCD_EN_ANS                  ANSELBbits.ANSB2
#define LCD_EN_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define LCD_EN_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define LCD_EN_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define LCD_EN_GetValue()           PORTBbits.RB2
#define LCD_EN_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define LCD_EN_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define LCD_EN_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define LCD_EN_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define LCD_EN_SetPushPull()        do { ODCONBbits.ODCB2 = 0; } while(0)
#define LCD_EN_SetOpenDrain()       do { ODCONBbits.ODCB2 = 1; } while(0)
#define LCD_EN_SetAnalogMode()      do { ANSELBbits.ANSB2 = 1; } while(0)
#define LCD_EN_SetDigitalMode()     do { ANSELBbits.ANSB2 = 0; } while(0)

// get/set LCD_RW aliases
#define LCD_RW_TRIS                 TRISBbits.TRISB3
#define LCD_RW_LAT                  LATBbits.LATB3
#define LCD_RW_PORT                 PORTBbits.RB3
#define LCD_RW_WPU                  WPUBbits.WPUB3
#define LCD_RW_OD                   ODCONBbits.ODCB3
#define LCD_RW_ANS                  ANSELBbits.ANSB3
#define LCD_RW_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define LCD_RW_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define LCD_RW_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define LCD_RW_GetValue()           PORTBbits.RB3
#define LCD_RW_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define LCD_RW_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define LCD_RW_SetPullup()          do { WPUBbits.WPUB3 = 1; } while(0)
#define LCD_RW_ResetPullup()        do { WPUBbits.WPUB3 = 0; } while(0)
#define LCD_RW_SetPushPull()        do { ODCONBbits.ODCB3 = 0; } while(0)
#define LCD_RW_SetOpenDrain()       do { ODCONBbits.ODCB3 = 1; } while(0)
#define LCD_RW_SetAnalogMode()      do { ANSELBbits.ANSB3 = 1; } while(0)
#define LCD_RW_SetDigitalMode()     do { ANSELBbits.ANSB3 = 0; } while(0)

// get/set LCD_RS aliases
#define LCD_RS_TRIS                 TRISBbits.TRISB4
#define LCD_RS_LAT                  LATBbits.LATB4
#define LCD_RS_PORT                 PORTBbits.RB4
#define LCD_RS_WPU                  WPUBbits.WPUB4
#define LCD_RS_OD                   ODCONBbits.ODCB4
#define LCD_RS_ANS                  ANSELBbits.ANSB4
#define LCD_RS_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define LCD_RS_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define LCD_RS_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define LCD_RS_GetValue()           PORTBbits.RB4
#define LCD_RS_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define LCD_RS_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define LCD_RS_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define LCD_RS_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define LCD_RS_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define LCD_RS_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define LCD_RS_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define LCD_RS_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set OLED_PWR aliases
#define OLED_PWR_TRIS                 TRISBbits.TRISB5
#define OLED_PWR_LAT                  LATBbits.LATB5
#define OLED_PWR_PORT                 PORTBbits.RB5
#define OLED_PWR_WPU                  WPUBbits.WPUB5
#define OLED_PWR_OD                   ODCONBbits.ODCB5
#define OLED_PWR_ANS                  ANSELBbits.ANSB5
#define OLED_PWR_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define OLED_PWR_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define OLED_PWR_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define OLED_PWR_GetValue()           PORTBbits.RB5
#define OLED_PWR_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define OLED_PWR_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define OLED_PWR_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define OLED_PWR_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define OLED_PWR_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define OLED_PWR_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define OLED_PWR_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define OLED_PWR_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set VSENS aliases
#define VSENS_TRIS                 TRISCbits.TRISC0
#define VSENS_LAT                  LATCbits.LATC0
#define VSENS_PORT                 PORTCbits.RC0
#define VSENS_WPU                  WPUCbits.WPUC0
#define VSENS_OD                   ODCONCbits.ODCC0
#define VSENS_ANS                  ANSELCbits.ANSC0
#define VSENS_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define VSENS_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define VSENS_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define VSENS_GetValue()           PORTCbits.RC0
#define VSENS_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define VSENS_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define VSENS_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define VSENS_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define VSENS_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define VSENS_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define VSENS_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define VSENS_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set ISENS aliases
#define ISENS_TRIS                 TRISCbits.TRISC1
#define ISENS_LAT                  LATCbits.LATC1
#define ISENS_PORT                 PORTCbits.RC1
#define ISENS_WPU                  WPUCbits.WPUC1
#define ISENS_OD                   ODCONCbits.ODCC1
#define ISENS_ANS                  ANSELCbits.ANSC1
#define ISENS_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define ISENS_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define ISENS_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define ISENS_GetValue()           PORTCbits.RC1
#define ISENS_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define ISENS_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define ISENS_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define ISENS_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define ISENS_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define ISENS_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define ISENS_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define ISENS_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set FAULT aliases
#define FAULT_TRIS                 TRISCbits.TRISC2
#define FAULT_LAT                  LATCbits.LATC2
#define FAULT_PORT                 PORTCbits.RC2
#define FAULT_WPU                  WPUCbits.WPUC2
#define FAULT_OD                   ODCONCbits.ODCC2
#define FAULT_ANS                  ANSELCbits.ANSC2
#define FAULT_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define FAULT_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define FAULT_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define FAULT_GetValue()           PORTCbits.RC2
#define FAULT_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define FAULT_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define FAULT_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define FAULT_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define FAULT_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define FAULT_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define FAULT_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define FAULT_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set PWR_EN aliases
#define PWR_EN_TRIS                 TRISCbits.TRISC3
#define PWR_EN_LAT                  LATCbits.LATC3
#define PWR_EN_PORT                 PORTCbits.RC3
#define PWR_EN_WPU                  WPUCbits.WPUC3
#define PWR_EN_OD                   ODCONCbits.ODCC3
#define PWR_EN_ANS                  ANSELCbits.ANSC3
#define PWR_EN_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define PWR_EN_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define PWR_EN_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define PWR_EN_GetValue()           PORTCbits.RC3
#define PWR_EN_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define PWR_EN_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define PWR_EN_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define PWR_EN_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define PWR_EN_SetPushPull()        do { ODCONCbits.ODCC3 = 0; } while(0)
#define PWR_EN_SetOpenDrain()       do { ODCONCbits.ODCC3 = 1; } while(0)
#define PWR_EN_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define PWR_EN_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set LCD_DB3 aliases
#define LCD_DB3_TRIS                 TRISCbits.TRISC4
#define LCD_DB3_LAT                  LATCbits.LATC4
#define LCD_DB3_PORT                 PORTCbits.RC4
#define LCD_DB3_WPU                  WPUCbits.WPUC4
#define LCD_DB3_OD                   ODCONCbits.ODCC4
#define LCD_DB3_ANS                  ANSELCbits.ANSC4
#define LCD_DB3_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define LCD_DB3_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define LCD_DB3_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define LCD_DB3_GetValue()           PORTCbits.RC4
#define LCD_DB3_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define LCD_DB3_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define LCD_DB3_SetPullup()          do { WPUCbits.WPUC4 = 1; } while(0)
#define LCD_DB3_ResetPullup()        do { WPUCbits.WPUC4 = 0; } while(0)
#define LCD_DB3_SetPushPull()        do { ODCONCbits.ODCC4 = 0; } while(0)
#define LCD_DB3_SetOpenDrain()       do { ODCONCbits.ODCC4 = 1; } while(0)
#define LCD_DB3_SetAnalogMode()      do { ANSELCbits.ANSC4 = 1; } while(0)
#define LCD_DB3_SetDigitalMode()     do { ANSELCbits.ANSC4 = 0; } while(0)

// get/set LCD_DB2 aliases
#define LCD_DB2_TRIS                 TRISCbits.TRISC5
#define LCD_DB2_LAT                  LATCbits.LATC5
#define LCD_DB2_PORT                 PORTCbits.RC5
#define LCD_DB2_WPU                  WPUCbits.WPUC5
#define LCD_DB2_OD                   ODCONCbits.ODCC5
#define LCD_DB2_ANS                  ANSELCbits.ANSC5
#define LCD_DB2_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define LCD_DB2_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define LCD_DB2_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define LCD_DB2_GetValue()           PORTCbits.RC5
#define LCD_DB2_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define LCD_DB2_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define LCD_DB2_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define LCD_DB2_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define LCD_DB2_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define LCD_DB2_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define LCD_DB2_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define LCD_DB2_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()               do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()                do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()                do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()       do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()      do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetPullup()             do { WPUCbits.WPUC6 = 1; } while(0)
#define RC6_ResetPullup()           do { WPUCbits.WPUC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()               do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()                do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()                do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()       do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()      do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetPullup()             do { WPUCbits.WPUC7 = 1; } while(0)
#define RC7_ResetPullup()           do { WPUCbits.WPUC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF4 pin functionality
 * @Example
    IOCAF4_ISR();
 */
void IOCAF4_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF4 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF4 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF4_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF4_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF4 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF4_SetInterruptHandler() method.
    This handler is called every time the IOCAF4 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF4_SetInterruptHandler(IOCAF4_InterruptHandler);

*/
extern void (*IOCAF4_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF4 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF4_SetInterruptHandler() method.
    This handler is called every time the IOCAF4 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF4_SetInterruptHandler(IOCAF4_DefaultInterruptHandler);

*/
void IOCAF4_DefaultInterruptHandler(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF5 pin functionality
 * @Example
    IOCAF5_ISR();
 */
void IOCAF5_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF5 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF5 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF5_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF5 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF5_SetInterruptHandler() method.
    This handler is called every time the IOCAF5 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(IOCAF5_InterruptHandler);

*/
extern void (*IOCAF5_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF5 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF5_SetInterruptHandler() method.
    This handler is called every time the IOCAF5 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(IOCAF5_DefaultInterruptHandler);

*/
void IOCAF5_DefaultInterruptHandler(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF7 pin functionality
 * @Example
    IOCAF7_ISR();
 */
void IOCAF7_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF7 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF7 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF7_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF7_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF7 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF7_SetInterruptHandler() method.
    This handler is called every time the IOCAF7 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF7_SetInterruptHandler(IOCAF7_InterruptHandler);

*/
extern void (*IOCAF7_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF7 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF7_SetInterruptHandler() method.
    This handler is called every time the IOCAF7 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF7_SetInterruptHandler(IOCAF7_DefaultInterruptHandler);

*/
void IOCAF7_DefaultInterruptHandler(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCCF2 pin functionality
 * @Example
    IOCCF2_ISR();
 */
void IOCCF2_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCCF2 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCCF2 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(MyInterruptHandler);

*/
void IOCCF2_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCCF2 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCCF2_SetInterruptHandler() method.
    This handler is called every time the IOCCF2 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(IOCCF2_InterruptHandler);

*/
extern void (*IOCCF2_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCCF2 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCCF2_SetInterruptHandler() method.
    This handler is called every time the IOCCF2 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(IOCCF2_DefaultInterruptHandler);

*/
void IOCCF2_DefaultInterruptHandler(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/