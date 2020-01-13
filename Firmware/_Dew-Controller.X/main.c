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
#include <math.h>
#include "mcc_generated_files/mcc.h"
#include "oled.h"

#define ABS(x) (x < 0 ? -1*x : x)

volatile uint32_t millis    = 0;
volatile uint8_t  duty      = 0; 
volatile uint8_t  pb_millis = 0;
volatile uint8_t  fault     = 0;
uint16_t voltage     = 0;
int16_t  ntc_temp    = 0;
int8_t   temp        = 0;
uint8_t  rh          = 0;
uint16_t total_power = 0;
uint8_t  dc[4]       = {0,0,0,0};
uint16_t power[4]    = {0,0,0,0};
uint16_t current[4]  = {0,0,0,0};

enum states       {start = 0, cw1, cw2, cw3, ccw1, ccw2, ccw3};
enum flags        {cw_flag = 0b10000000, ccw_flag = 0b01000000};
enum direction    {stop, cw, ccw};
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
volatile enum direction dir   = stop;
volatile enum button_press pb = none;

typedef struct {
	uint16_t temp_C_code;
	uint16_t RH_code;
    uint16_t status;
} DATA_STRUCT;

typedef union {
    DATA_STRUCT s;
    uint8_t raw[sizeof(DATA_STRUCT)];
} DATA_OBJ;

void timer0() {
    millis++;
}

void rot() {
    uint8_t input;

	// input code for rot_a and rot_b combined
    input = (ROT_B_PORT << 1) | ROT_A_PORT;	
	// set current state according to transition table (cw/ccw flags masked out)
    current_state = transition_table[current_state & 0b00000111][input];
	// set global direction flag
	// dir must be set to 'stop' inside application code after evaluation
    if (current_state & cw_flag) dir = cw;
    if (current_state & ccw_flag) dir = ccw;
}

void pushbutton() {
	// reset millisecond tick upon first rising edge of push button 
    if (!ROT_PB_PORT) {
        millis = 0;
    } else {
        pb_millis = millis;
		// short button press
        if ((millis > 5) & (millis <= 50)) pb = short_press;
		// long button press
        else if ((millis > 50) & (millis <= 150)) {
            pb = long_press;
        }
		// button pushed down too long => abort
        else pb = abort;
    }
}

void overcurrent() {
    PWR_EN_LAT = 0;
    TMR1_StopTimer();
    SW_CH1_LAT = 0;
    SW_CH2_LAT = 0;
    SW_CH3_LAT = 0;
    SW_CH4_LAT = 0;
    fault = 1;
}

void read_ntc_temp(void) {
    uint32_t val;
    
	// read aux. ntc temp sensor
    val = ADC_GetConversion(TEMP);
    ntc_temp = (uint16_t)(((val * 119) - 34511) / 100);
	
	
	// TODO:
	// Not connected?
	
}

uint16_t get_battery_volt(void) {
    uint32_t val;
    val = ADC_GetConversion(VSENS);
    return (uint16_t)((val << 11) / 10000);
}

uint16_t get_current(void) {
    uint32_t val;
    val = ADC_GetConversion(ISENS);
    return (uint16_t)((val * 1955) / 1000);
}
    

void read_cur_volt(void) {
	uint8_t i;
	uint16_t volt, volt_min;
	
	// read current CH1
	SW_CH1_LAT = 1;
    current[0] = get_current();
	// read battery voltage under load
    volt_min = get_battery_volt();
	SW_CH1_LAT = 0;
	
	// read current CH2
	SW_CH2_LAT = 1;
    current[1] = get_current();
	// read battery voltage under load
	volt = get_battery_volt();
	if (volt < volt_min) volt_min = volt;
	SW_CH2_LAT = 0;
	
	// read current CH3
	SW_CH3_LAT = 1;
    current[2] = get_current();
	// read battery voltage under load
	volt = get_battery_volt();
	if (volt < volt_min) volt_min = volt;
	SW_CH3_LAT = 0;
	
	// read current CH4
	SW_CH4_LAT = 1;
    current[3] = get_current();
	// read battery voltage under load
	volt = get_battery_volt();
	if (volt < volt_min) volt_min = volt;
	SW_CH4_LAT = 0;
	
	// use lowest voltage reading for display
	voltage = volt_min;
	
	// totalize power
	total_power = 0;
    for(i=0;i<4;i++) {
        power[i] = volt * (uint32_t)current[i] / (uint32_t)1000;
        total_power += power[i];
    }
}

void timer1() {
    if (duty++ > 99) {
		duty = 0;
		// Stop Timer 1 and turn all channels off for current measurement
		TMR1_StopTimer();
		SW_CH1_LAT = 0;
		SW_CH2_LAT = 0;
		SW_CH3_LAT = 0;
		SW_CH4_LAT = 0;
	} else {
		// turn channels on according to set duty cycle
		if (duty < dc[0]) SW_CH1_LAT = 1; else SW_CH1_LAT = 0;
		if (duty < dc[1]) SW_CH2_LAT = 1; else SW_CH2_LAT = 0;
		if (duty < dc[2]) SW_CH3_LAT = 1; else SW_CH3_LAT = 0;
		if (duty < dc[3]) SW_CH4_LAT = 1; else SW_CH4_LAT = 0;
	}
}

uint8_t check_status() {
	if (voltage <= 104) {
        OLED_returnHome();
        OLED_command(LCD_CLEARDISPLAY);
        OLED_print_xy(0,0, "Main B bus");
        OLED_print_xy(0,1, "undervolt.");
        return 1;
	} else if ((voltage > 132) && (voltage <= 144)) {
        OLED_returnHome();
        OLED_command(LCD_CLEARDISPLAY);
        OLED_print_xy(0,0, "Main B bus");
        OLED_print_xy(0,1, "overvolt.");
        return 1;
	} else if (voltage > 144) {
		// Voltage too high => disable load switch and OLED power
        OLED_command(0x08);
		OLED_PWR_LAT = 0;
		PWR_EN_LAT = 0;
        LCD_DB0_LAT = 0;
        LCD_DB0_LAT = 0;
        LCD_DB1_LAT = 0;
        LCD_DB2_LAT = 0;
        LCD_DB3_LAT = 0;
        LCD_RS_LAT = 0;
        LCD_EN_LAT = 0;
        while(1);
		// run in circles, scream and shout
	}
    
    if (fault) {
        OLED_returnHome();
        pb = none;
        while(pb != abort) {
            OLED_command(LCD_CLEARDISPLAY);
            OLED_print_xy(0,0, "Output");
            OLED_print_xy(0,1, "overloaded");
            __delay_ms(1500);
            OLED_command(LCD_CLEARDISPLAY);
            OLED_print_xy(0,0, "Remove short");
            OLED_print_xy(0,1, "circuit");
            __delay_ms(1500);
            OLED_command(LCD_CLEARDISPLAY);
            OLED_print_xy(0,0, "Hold button");
            OLED_print_xy(0,1, "to reset");
            __delay_ms(1500);
        }
        fault = 0;
        PWR_EN_LAT = 1;
        TMR1_StartTimer();
    }
    return 0;
	
	
}

void main_menu(void) {
    static uint8_t menu = 0; 
	static uint8_t page = 0;
	uint8_t pages       = 0;
    uint8_t i;
	
    char s[61];
	
	if (menu == 0)
	{
		// Main menu
		pages = 3;
		OLED_print_xy(0,0, "Temperatur  Feuchte     Taupunkt    Batterie");
		sprintf(s,"%3d,%d \xdf\C    %3d,%d %%rel    -xx,x \xdf\C    %2u,%1uV  %2u,%uW", 
				(int8_t)(temp/10),(int8_t)(ABS(temp)%10), 
                (uint8_t)(rh/10),(uint8_t)(rh%10),
				(uint8_t)(voltage / 10),(uint8_t)(voltage % 10), 
				total_power / 10, total_power % 10);
		OLED_print_xy(0,1, s);
	}
	else if (menu == 1)
	{
		// Display power for each channel
		pages = 3;
		OLED_print_xy(0,0, "Kanal 1     Kanal 2     Kanal 3     Kanal 4");
		sprintf(s,"%2d %%  %2u,%u W%2d %%  %2u,%u W",
				dc[0], power[0]/10, power[0]%10, dc[1], power[1]/10, power[1]%10);
		OLED_print_xy(0,1, s);
		// Long button press enters channel setup
		if (pb == long_press) {
			menu = 10;
			pb = none;
		}
	}
	else if (menu == 10)
	{
		// Channel setup
		// Channel number = page + 1
		OLED_print_xy(12 * page, 0, "Tastgrad");
		sprintf(s, "Kanal %d: ", page + 1);
		OLED_print_xy(12 * page, 1, s);

		while(pb == none) {
			if ((dir == cw) && (dc[page] < 99)) {
				dc[page]++;
				dir = stop;
			} else if ((dir == ccw) && (dc[page] > 0)) {
				dc[page]--;
				dir = stop;
			}
			sprintf(s,"%2d%%", dc[page]);
			OLED_print_xy(12 * page + 9, 1, s);
		}
		pb = none;
		// return to channel power display
		menu = 1;
	}
	
	// short button press traverses through main menu
	if (pb == short_press) {
		pb = none;
		if (menu == 0) menu = 1;
		else if (menu == 1) menu = 0;
		page  = 0;
		OLED_returnHome();
	}
	
	// rotary encoder changes visible page only inside main menu (menu < 10)
	if (menu < 10) {
		if ((dir == cw) && (page < pages)) {
			page++;
			dir = stop;
			for(i=0;i<12;i++) {
				OLED_scrollDisplayLeft();
				__delay_ms(20);
			}
		}
		else if ((dir == ccw) && (page > 0)) {
			page--;
			dir = stop;
			for(i=0;i<12;i++) {
				OLED_scrollDisplayRight();
				__delay_ms(20);
			}
		}

	}
}

/*
                         Main application
 */
void main(void)
{
    uint8_t i;
    uint16_t uRH, utemp_C,ustat;
    float fRH, ftemp_C, ftd, fH;
    DATA_OBJ obj;
            
    // initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    ADC_Initialize();
	
	// turn on OLED power
	OLED_PWR_LAT = 1;
	
	// turn on channel load switch
	PWR_EN_LAT = 1;
 
	// initialize display
    OLED_init();
    
    read_cur_volt();
    
	// Set interrupt handler
    IOCAF7_SetInterruptHandler(pushbutton); // Rotary encooder push button int
    IOCAF4_SetInterruptHandler(rot);        // Rotary encoder pin A int
    IOCAF5_SetInterruptHandler(rot);        // Rotary encoder pin B int
    TMR0_SetInterruptHandler(timer0);		// Millisecond tick
    TMR1_SetInterruptHandler(timer1);		// Slow duty cycle timer 
    IOCCF2_SetInterruptHandler(overcurrent);// Overcurrent fault
   
    dc[0] = 50;
    
    
    while (1)
    {
		// read aux. ntc sensor
        read_ntc_temp();
        
        if (eusart1RxCount >= 6) {
//            for(i=0;i<6;i++) {
//                obj.raw[i] = EUSART1_Read();
//            }
            i = EUSART1_Read(); uRH = i;
            i = EUSART1_Read(); uRH |= i<<8;
            
            i = EUSART1_Read(); utemp_C = i;
            i = EUSART1_Read(); utemp_C |= i<<8;
            
            i = EUSART1_Read(); ustat = i;
            i = EUSART1_Read(); ustat |= i<<8;
            
            // convert rh and temp codes
//            fRH = (125.0 * obj.s.RH_code) / 65536 - 6;
//            ftemp_C = (175.72 * obj.s.temp_C_code) / 65536 - 46.85;
            fRH = (125.0 * uRH) / 65536 - 6;
            ftemp_C = (175.72 * utemp_C) / 65536 - 46.85;
         
            // calculate dew point temperature
            fH = (log10(fRH) - 2.0) / 0.4343 + (17.62 * ftemp_C) / (243.12 + ftemp_C);
            ftd = 243.12 * fH / (17.62 - fH);
            
            if (fRH < 0) fRH = 0;
            else if (fRH > 100) fRH = 100;
            rh = round(fRH * 10.0);
            temp = round(ftemp_C * 10.0);
            NOP();
        }
		
		// do current measurement at duty cycle roll over
		if (duty == 0) {
			read_cur_volt();
			TMR1_StartTimer();
		}
		
		// check system health 
        if (check_status() == 0) {
            // Display menu
            main_menu();
        }


        
        __delay_ms(100);

    }
}
/**
 End of File
*/