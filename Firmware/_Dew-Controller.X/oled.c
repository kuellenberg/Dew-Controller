/*
 * File:   oled.c
 * Author: André Küllenberg
 *
 * Created on December 21, 2018, 9:51 PM
 */


#include <xc.h>
#include <stdlib.h>
#include "oled.h"
#include "mcc_generated_files/pin_manager.h"

void OLED_pulseEnable(void) 
{
    LCD_EN_LAT = 1;
    __delay_us(50);
    LCD_EN_LAT = 0;
}

void OLED_write4bits(uint8_t value) 
{
    LCD_DB0_LAT = (value >> 0) & 0x01;
    LCD_DB1_LAT = (value >> 1) & 0x01;
    LCD_DB2_LAT = (value >> 2) & 0x01;
    LCD_DB3_LAT = (value >> 3) & 0x01;
    
    __delay_us(50); // Timing spec?
    OLED_pulseEnable();
}

void OLED_send(uint8_t value, uint8_t mode) 
{
    LCD_RS_LAT = mode;
    LCD_RW_LAT = 0;

    OLED_write4bits(value>>4);
    OLED_write4bits(value);
}

// Poll the busy bit until it goes LOW
void OLED_waitForReady(void) 
{
  
    unsigned char busy = 1;
    
    LCD_DB3_TRIS = 1; // DB3 = busy-pin = input
    LCD_RS_LAT = 0;
    LCD_RW_LAT = 1;
  
    do
    {
        LCD_EN_LAT = 0;
        __delay_us(10);
        LCD_EN_LAT = 1;
      
        __delay_us(10);
        busy = LCD_DB3_PORT;
      
        LCD_EN_LAT = 0;
  	
        OLED_pulseEnable();		// get remaining 4 bits, which are not used.
    }
    while(busy);
  
    LCD_DB3_TRIS = 0;
    LCD_RW_LAT = 0;
}

void OLED_command(uint8_t value) 
{
    OLED_send(value, LOW);
    OLED_waitForReady();
}

void OLED_write(uint8_t value) 
{
    OLED_send(value, HIGH);
    OLED_waitForReady();
}

void OLED_init(void) 
{
    LCD_RS_LAT = 0;
    LCD_EN_LAT = 0;
    LCD_RW_LAT = 0;
    
    __delay_ms(50);
  
  // Now we pull both RS and R/W low to begin commands
  
    LCD_DB0_LAT = 0;
    LCD_DB1_LAT = 0;
    LCD_DB2_LAT = 0;
    LCD_DB3_LAT = 0;

    // Initialization sequence is not quite as documented by Winstar.
    // Documented sequence only works on initial power-up.  
    // An additional step of putting back into 8-bit mode first is 
    // required to handle a warm-restart.
    //
    // In the data sheet, the timing specs are all zeros(!).  These have been tested to 
    // reliably handle both warm & cold starts.

    // 4-Bit initialization sequence from Technobly
    OLED_write4bits(0x03); // Put back into 8-bit mode
    __delay_ms(5);
    OLED_write4bits(0x08); // only run extra command for newer displays
    __delay_ms(5);

    OLED_write4bits(0x02); // Put into 4-bit mode
    __delay_ms(5);
    OLED_write4bits(0x02);
    __delay_ms(5);
    OLED_write4bits(0x08);
    __delay_ms(5);

    OLED_command(0x08);	// Turn Off
    __delay_ms(5);
    OLED_command(0x01);	// Clear Display
    __delay_ms(5);
    OLED_command(0x06);	// Set Entry Mode
    __delay_ms(5);
    OLED_command(0x02);	// Home Cursor
    __delay_ms(5);
    OLED_command(0x0C);	// Turn On - enable cursor & blink
    __delay_ms(5);
}

void OLED_scrollDisplayLeft(void) 
{
    OLED_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void OLED_scrollDisplayRight(void) 
{
    OLED_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void OLED_setCursor(uint8_t col, uint8_t row)
{
  uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
 
  OLED_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void OLED_print(char *s)
{
    while (*s != NULL) {
        OLED_write(*s++);
    }
}

void OLED_print_xy(uint8_t col, uint8_t row, char *s) {
    uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

    OLED_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
    while (*s != NULL) {
        OLED_write(*s++);
    }
}

void OLED_returnHome(void) {
    OLED_command(LCD_RETURNHOME);
}