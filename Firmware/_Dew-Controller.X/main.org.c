/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F15354
        Driver Version    :  2.00
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

#include <stdio.h>
#include "mcc_generated_files/mcc.h"
#include "oled.h"
#include <math.h>

#define ABS(x) (x < 0 ? -1*x : x)

volatile uint32_t millis = 0;
volatile uint8_t duty = 0; 
volatile uint8_t pb_millis = 0;
uint8_t dc[4] = {0,0,0,0};
uint16_t volt;
int16_t temp;
uint16_t total;
uint16_t power[4] = {0,0,0,0};
uint16_t current[4] = {0,0,0,0};

enum states {start = 0, cw1, cw2, cw3, ccw1, ccw2, ccw3};
enum flags {cw_flag = 0b10000000, ccw_flag = 0b01000000};
enum direction {stop, cw, ccw};
enum button_press {none, short_press, long_press, abort};

// transition table for encoder FSM 
const uint8_t transition_table[7][4] = {
/*                 00           01      10      11  */
/* -----------------------------------------------------*/
/*START     | */  {start,       ccw1,   cw1,    start},
/*CW_Step1  | */  {cw2|cw_flag, start,  cw1,    start},
/*CW_Step2  | */  {cw2,         cw3,    cw1,    start},
/*CW_Step3  | */  {cw2,         cw3,    start,  start|cw_flag},
/*CCW_Step1 | */  {ccw2|ccw_flag,ccw1,  start,  start},
/*CCW_Step2 | */  {ccw2,        ccw1,   ccw3,   start},
/*CCW_Step3 | */  {ccw2,        start,  ccw3,   start|ccw_flag}
};


volatile enum states current_state = start;
volatile enum direction dir = stop;
volatile enum button_press pb = none;

void timer0() {
    millis++;
}


void rot() {
    uint8_t input;

    input = (ROT_B_PORT << 1) | ROT_A_PORT;
    current_state = transition_table[current_state & 0b00000111][input];
    if (current_state & cw_flag) dir = cw;
    if (current_state & ccw_flag) dir = ccw;
}

void pushbutton() {
    if (!ROT_PB_PORT) {
        millis = 0;
    } else {
        pb_millis = millis;
        if ((millis > 5) & (millis <= 50)) pb = short_press;
        else if ((millis > 50) & (millis <= 150)) pb = long_press;
        else pb = abort;
    }
}

void daq(void) {
    uint8_t i;
    uint32_t val;
    
    val = ADC_GetConversion(TEMP);
    temp = (uint16_t)(((val * 119) - 34511) / 100);

    val = ADC_GetConversion(VSENS);
    volt = (uint16_t)((val << 11) / 10000);

    val = ADC_GetConversion(ISENS);
    current[0] = (uint16_t)((val * 1955) / 1000);

    total = 0;
    for(i=0;i<4;i++) {
        power[i] = volt * current[i] / 1000;
        total += power[i];
    }
}

void timer1() {
    if (duty++ > 99) duty = 0;
    if (duty < dc[0]) SW_CH1_LAT = 1; else SW_CH1_LAT = 0;
//    if (duty < dc2) SW_CH2_LAT = 1; else SW_CH2_LAT = 0;
//    if (duty < dc3) SW_CH3_LAT = 1; else SW_CH3_LAT = 0;
//    if (duty < dc4) SW_CH4_LAT = 1; else SW_CH4_LAT = 0;
    
}

/*
                         Main application
 */
void main(void)
{
    int8_t i = 0;

    char s[61];
    uint8_t screen = 0, screen_max = 0;
    uint8_t menu = 0;    

    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    OLED_PWR_LAT = 1;
    ADC_Initialize();
     
    OLED_init();
    
    IOCAF7_SetInterruptHandler(pushbutton);  // ROT_PB
    IOCAF4_SetInterruptHandler(rot);    // ROT_A
    IOCAF5_SetInterruptHandler(rot);    // ROT_B
    TMR0_SetInterruptHandler(timer0);
    TMR1_SetInterruptHandler(timer1);
    
    menu = 0;
    screen = 0;
    screen_max = 3;
   
    dc[0] = 50;
    
    while (1)
    {
        daq();
        
        if (menu == 0) {
            OLED_print_xy(0,0, "Temperatur  Feuchte     Taupunkt    Batterie");
            sprintf(s,"%3d,%d \xdf\C    xxx %%rel    -xx,x \xdf\C    %2u,%1uV  %2u,%uW", 
                    (int8_t)(temp/10),(int8_t)(ABS(temp)%10), 
                    (uint8_t)(volt/10),(uint8_t)(volt%10), 
                    total/10, total%10);
            OLED_print_xy(0,1, s);
        } else if (menu == 1) {
            OLED_print_xy(0,0, "Kanal 1     Kanal 2     Kanal 3     Kanal 4");
            sprintf(s,"%2d %%  %2u,%u W%2d %%  %2u,%u W",
                    dc[0], power[0]/10, power[0]%10, dc[1], power[1]/10, power[1]%10);
            OLED_print_xy(0,1, s);
            if (pb == long_press) {
                menu = 10;
                pb = none;
            }
        } else if (menu == 10) {
            OLED_print_xy(12*screen,0, "Tastgrad");
            sprintf(s,"Kanal %d: ", screen + 1);
            OLED_print_xy(12*screen,1, s);

            while(pb == none) {
                if ((dir == cw) && (dc[screen] < 99)) {
                    dc[screen]++;
                    dir = stop;
                } else if ((dir == ccw) && (dc[screen] > 0)) {
                    dc[screen]--;
                    dir = stop;
                }
                sprintf(s,"%2d%%", dc[screen]);
                OLED_print_xy(12*screen+9,1, s);
            }
            pb = none;
            menu = 1;
        }
        
        if (pb == short_press) {
            pb = none;
            if (menu == 0) { menu = 1; screen_max = 1; }
            else if (menu == 1) { menu = 0; screen_max = 3; }
            screen = 0;
            OLED_returnHome();
        }
        
        if (menu < 10) {
            if ((dir == cw) && (screen < screen_max)) {
                screen++;
                dir = stop;
                for(i=0;i<12;i++) {
                    OLED_scrollDisplayLeft();
                    __delay_ms(20);
                }
            }
            else if ((dir == ccw) && (screen > 0)) {
                screen--;
                dir = stop;
                for(i=0;i<12;i++) {
                    OLED_scrollDisplayRight();
                    __delay_ms(20);
                }
            }

        }
        __delay_ms(100);
    }
}
/**
 End of File
*/