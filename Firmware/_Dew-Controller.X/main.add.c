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
#include <string.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/eusart1.h"
#include "oled.h"

#define ABS(x) (x < 0 ? -1*x : x)

#define CHANNELS 2
#define RECV_BUF_SIZE 20

enum channel_mode {manual, magic};
enum channel_state {ok, open, overload};

struct channel {
    int16_t dc;
    uint16_t current;
    uint16_t power;
    enum channel_mode mode;
    enum channel_state state;
};

volatile uint32_t millis = 0;
volatile uint8_t duty = 0; 
volatile uint8_t pb_millis = 0;
uint16_t volt;
int16_t temp, soll = 0;
uint16_t total;
struct channel ch[CHANNELS];

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

volatile uint8_t recv_buf[RECV_BUF_SIZE];
volatile uint8_t recv_count = 0;
volatile uint8_t rx_ready = 0;

void timer0() {
    millis++;
}

void rot() {
    current_state = transition_table[current_state & 0b00000111][(ROT_B_PORT << 1) | ROT_A_PORT];
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

    val = ADC_GetConversion(ISENS_CH1);
    ch[0].current = (uint16_t)((val * 1955) / 1000);

    total = 0;
    for(i=0;i<CHANNELS;i++) {
        if (ch[i].state == overload) continue;
        
        if (ch[i].current < 10) ch[i].state = open;
        else if (ch[i].current > 1000) {
            ch[i].state = overload;
            ch[i].mode = manual;
            ch[i].dc = 0;
            ch[i].current = 0;
        } else {
            ch[i].state = ok;
        }
        val = (uint32_t)volt * (uint32_t)ch[i].current / (uint32_t)1000;
        ch[i].power = (uint16_t)val;
        total += ch[i].power;
    }
}

void timer1() {
    uint8_t i;
   
    if (duty++ > 99) duty = 0;
    if (duty < ch[0].dc) SW_CH1_LAT = 1; else SW_CH1_LAT = 0;
//    if (duty < dc2) SW_CH2_LAT = 1; else SW_CH2_LAT = 0;
//    if (duty < dc3) SW_CH3_LAT = 1; else SW_CH3_LAT = 0;
//    if (duty < dc4) SW_CH4_LAT = 1; else SW_CH4_LAT = 0;
    
    if (duty % 5 == 0) daq();
    
    if (duty % 50 == 0) {
        for(i=0;i<CHANNELS;i++) {
            if (ch[i].mode == magic) {
                if (temp < soll*10-5) ch[i].dc = 100;
                else ch[i].dc = 0;
            }
        }
    }
}

void recv(void)
{    
    if(1 == RC1STAbits.OERR)
    {
        // EUSART1 error - restart

        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
    }

    if ((recv_count > RECV_BUF_SIZE) || (RC1REG == '*')) {
        recv_count = 0;
    } else if (RC1REG == '#') {
        recv_buf[recv_count++] = NULL;
        rx_ready = 1;
    } else {
        recv_buf[recv_count++] = RC1REG;
    }
}

/*
                         Main application
 */
void main(void)
{
    int8_t i = 0;
    int8_t n;
    char s[50], sc[8];
    uint8_t screen = 0, screen_max = 3;
    uint8_t menu = 0;    
    uint16_t t1, h1, d1;
    char *c, *d;

    // initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
     
    OLED_init();
    
    IOCCF1_SetInterruptHandler(pushbutton);  // ROT_PB
    IOCAF6_SetInterruptHandler(rot);    // ROT_A
    IOCCF0_SetInterruptHandler(rot);    // ROT_B
    TMR0_SetInterruptHandler(timer0);
    TMR1_SetInterruptHandler(timer1);
    EUSART1_SetRxInterruptHandler(recv);
    
   
    for(i=0;i<CHANNELS;i++) {
        ch[i].current = 0;
        ch[i].dc = 0;
        ch[i].mode = magic;
        ch[i].power = 0;
        ch[i].state = open;
    }
    soll = 30;
    
    while (1)
    {
        switch(menu) {
            case 0:
                OLED_print_xy(0,0, "Temperatur  Feuchte     Taupunkt    Batterie");
                sprintf(s,"%3d,%d \xdf\C    %3d,%d %%rel  %3d,%d \xdf\C    %2u,%1uV  %2u,%uW", 
                        (int8_t)(t1/10),(int8_t)(ABS(t1)%10), 
                        (int8_t)(h1/10),(int8_t)(ABS(h1)%10), 
                        (int8_t)(d1/10),(int8_t)(ABS(d1)%10), 
                        (uint8_t)(volt/10),(uint8_t)(volt%10), 
                        total/10, total%10);
                OLED_print_xy(0,1, s);
                break;
            case 1:
                for(i=0;i<CHANNELS;i++) {
                    sprintf(s, "Kanal %d     ", i+1);
                    OLED_print_xy(i*12,0, s);
                    if (ch[i].state == ok) {
                        if (ch[i].mode == magic) sprintf(sc, "auto");
                        else sprintf(sc, "%3d %%", ch[i].dc);
                    } else {
                        if (ch[i].state == open) sprintf(sc, "----");
                        else if (ch[i].state == overload) sprintf(sc, "Fehler");
                    }
                    
                    sprintf(s, "%5s %2u,%u W", sc, ch[i].power/10, ch[i].power%10);
                    OLED_print_xy(i*12,1, s);
                }
                if (pb == long_press) {
                    menu = 10;
                    pb = none;
                }
                break;
            case 2:
                OLED_print_xy(0,0,"Solltemp.   Kp");
                sprintf(s,"%3d \xdf\C      ", soll);
                OLED_print_xy(0,1, s);
                if (pb == long_press) {
                    menu = 11;
                    pb = none;
                }
                break;
            case 10:
                OLED_print_xy(12*screen,0, "Tastgrad");
                sprintf(s,"Kanal%d: ", screen + 1);
                OLED_print_xy(12*screen,1, s);
                if (ch[screen].mode == manual) n = ch[screen].dc;
                else n = 101;
                while(pb == none) {
                    if ((dir == cw) && (n < 101)) {
                        n++;
                        dir = stop;
                    } else if ((dir == ccw) && (n > 0)) {
                        n--;
                        dir = stop;
                    }
                    if (n <= 100) sprintf(s,"%3d%%", n);
                    else sprintf(s,"auto");
                    OLED_print_xy(12*screen+8,1, s);
                }
                if (n <= 100) {
                    ch[screen].dc = n;
                    ch[screen].mode = manual;
                } else {
                    ch[screen].mode = magic;
                }
                pb = none;
                dir = none;
                menu = 1;
                break;
            case 11:
                n = soll;
                while(pb == none) {
                    if ((dir == cw) && (n < 40)) {
                        n++;
                        dir = stop;
                    } else if ((dir == ccw) && (n > -40)) {
                        n--;
                        dir = stop;
                    }
                    sprintf(s,"%3d \xdf\C", n);                    
                    OLED_print_xy(0,1, s);                    
                }
                soll = n;
                pb = none;
                dir = none;
                menu = 2;
                break;
            default:
                menu = 0;
                screen = 0;
                OLED_returnHome();
                break;
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
        
        if (pb == short_press) {
            pb = none;
            switch (menu) {
                case 0:
                    menu = 1; 
                    screen_max = 1;
                    break;
                case 1:
                    menu = 2; 
                    screen_max = 0;
                    break;
                case 2:
                    menu = 0;
                    screen_max = 3;
                default:
                    menu = 0;
                    screen_max = 3;
            }
            screen = 0;
            dir = none; // weil sonst zuckt
            OLED_returnHome();
        }
        
        if (rx_ready == 1) {
            c = strchr(recv_buf,';');
            if (c) {
                *c = NULL;
                t1 = atoi(recv_buf);
            }
            c++;
            d = strchr(c,';');
            if (d) {
                *d = NULL;
                h1 = atoi(c);
                d++;
                d1 = atoi(d);
            }
            rx_ready = 0;

        }
        __delay_ms(100);
    }
}
/**
 End of File
*/