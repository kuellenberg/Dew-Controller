// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef OLED_H
#define	OLED_H

#include "common.h"

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

#endif