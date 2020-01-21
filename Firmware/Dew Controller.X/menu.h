/* 
 * File:   menu.h
 * Author: User
 *
 * Created on 21. Januar 2020, 15:56
 */

#ifndef MENU_H
#define	MENU_H

void menu(t_globalData *data);
void menuInput(uint8_t *page, const uint8_t numPages, uint8_t *menu, 
	uint8_t pbShort, uint8_t pbLong, uint8_t timeout);

#endif	/* MENU_H */

