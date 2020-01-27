/*
 * File:   oled.c
 * Author: André Küllenberg
 *
 * Created on December 21, 2018, 9:51 PM
 */


#include "common.h"
#include "oled.h"

void OLED_Off(void)
{
	OLED_command(0x08);
	OLED_command(0x13);
	OLED_PWR = 1;
}

void OLED_pulseEnable(void)
{
	OLED_EN = 1;
	__delay_us(50);
	OLED_EN = 0;
}

void OLED_write4bits(uint8_t value)
{
	OLED_DB0 = (value >> 0) & 0x01;
	OLED_DB1 = (value >> 1) & 0x01;
	OLED_DB2 = (value >> 2) & 0x01;
	OLED_DB3 = (value >> 3) & 0x01;

	__delay_us(50); // Timing spec?
	OLED_pulseEnable();
}

void OLED_send(uint8_t value, uint8_t mode)
{
	OLED_RS = mode;
	OLED_RW = 0;

	OLED_write4bits(value >> 4);
	OLED_write4bits(value);
}

// Poll the busy bit until it goes LOW

void OLED_waitForReady(void)
{
	unsigned char busy = 1;

	OLED_DB3_T = 1; // DB3 = busy-pin = input
	OLED_RS = 0;
	OLED_RW = 1;

	do {
		OLED_EN = 0;
		__delay_us(10);
		OLED_EN = 1;

		__delay_us(10);
		busy = OLED_DB3_IN;

		OLED_EN = 0;

		OLED_pulseEnable(); // get remaining 4 bits, which are not used.
	} while (busy);

	OLED_DB3_T = 0;
	OLED_RW = 0;
}

void OLED_command(uint8_t value)
{
	OLED_send(value, 0);
	OLED_waitForReady();
}

void OLED_write(uint8_t value)
{
	OLED_send(value, 1);
	OLED_waitForReady();
}

void OLED_init(void)
{
	OLED_RS = 0;
	OLED_EN = 0;
	OLED_RW = 0;

	__delay_ms(50);

	// Now we pull both RS and R/W low to begin commands

	OLED_DB0 = 0;
	OLED_DB1 = 0;
	OLED_DB2 = 0;
	OLED_DB3 = 0;

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

	OLED_command(0x08); // Turn Off
	__delay_ms(5);
	OLED_command(0x01); // Clear Display
	__delay_ms(5);
	OLED_command(0x06); // Set Entry Mode
	__delay_ms(5);
	OLED_command(0x02); // Home Cursor
	__delay_ms(5);
	OLED_command(0x0C); // Turn On - enable cursor & blink
	__delay_ms(5);
}

void OLED_scrollDisplayLeft(void)
{
	OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVELEFT);
}

void OLED_scrollDisplayRight(void)
{
	OLED_command(OLED_CURSORSHIFT | OLED_DISPLAYMOVE | OLED_MOVERIGHT);
}

void OLED_setCursor(uint8_t col, uint8_t row)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	OLED_command(OLED_SETDDRAMADDR | (col + row_offsets[row]));
}

void OLED_print(char *s)
{
	while (*s != (char) NULL) {
		OLED_write(*s++);
	}
}

void OLED_print_xy(uint8_t col, uint8_t row, char *s)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	OLED_command(OLED_SETDDRAMADDR | (col + row_offsets[row]));
	while (*s != (char) NULL) {
		OLED_write(*s++);
	}
}

void OLED_returnHome(void)
{
	OLED_command(OLED_RETURNHOME);
}

void OLED_clearDisplay(void)
{
	OLED_command(OLED_CLEARDISPLAY);
}

void OLED_loadSpecialChars(void)
{
	uint8_t special[] = {0x18, 0x18, 0x03, 0x04, 0x04, 0x04, 0x03, 0x00, // °C
			     0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, // center dot
			     0x01, 0x01, 0x05, 0x09, 0x1F, 0x08, 0x04, 0x00}; // back arrow
	uint8_t n;
	
	OLED_command(OLED_SETCGRAMADDR + 8);
	for(n = 0; n < sizeof(special); n++) {
		OLED_write(special[n]);
	}
}