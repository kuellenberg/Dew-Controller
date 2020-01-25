// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef OLED_H
#define	OLED_H

#include "common.h"

// commands
#define OLED_CLEARDISPLAY    0x01
#define OLED_RETURNHOME      0x02
#define OLED_ENTRYMODESET    0x04
#define OLED_DISPLAYCONTROL  0x08
#define OLED_CURSORSHIFT     0x10
#define OLED_FUNCTIONSET     0x20
#define OLED_SETCGRAMADDR    0x40
#define OLED_SETDDRAMADDR    0x80

// flags for display entry mode
#define OLED_ENTRYRIGHT          0x00
#define OLED_ENTRYLEFT           0x02
#define OLED_ENTRYSHIFTINCREMENT 0x01
#define OLED_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define OLED_DISPLAYON   0x04
#define OLED_DISPLAYOFF  0x00
#define OLED_CURSORON    0x02
#define OLED_CURSOROFF   0x00
#define OLED_BLINKON     0x01
#define OLED_BLINKOFF    0x00

// flags for display/cursor shift
#define OLED_DISPLAYMOVE 0x08
#define OLED_CURSORMOVE  0x00
#define OLED_MOVERIGHT   0x04
#define OLED_MOVELEFT    0x00

// flags for function set
#define OLED_8BITMODE    0x10
#define OLED_4BITMODE    0x00
#define OLED_JAPANESE    0x00
#define OLED_EUROPEAN_I  0x01
#define OLED_RUSSIAN     0x02
#define OLED_EUROPEAN_II 0x03

void OLED_pulseEnable(void);
void OLED_write4bits(uint8_t value);
void OLED_send(uint8_t value, uint8_t mode);
void OLED_waitForReady(void);
void OLED_command(uint8_t value);
void OLED_write(uint8_t value);
void OLED_init(void);
void OLED_scrollDisplayLeft(void);
void OLED_scrollDisplayRight(void);
void OLED_print(char *s);
void OLED_print_xy(uint8_t col, uint8_t row, char *s);
void OLED_setCursor(uint8_t col, uint8_t row);
void OLED_returnHome(void);
void OLED_clearDisplay(void);
void OLED_loadSpecialChars(void);

#endif