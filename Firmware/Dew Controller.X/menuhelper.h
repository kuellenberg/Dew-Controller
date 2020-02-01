/* 
 * File:   menu.h
 * Author: User
 *
 * Created on 21. Januar 2020, 15:56
 */

#ifndef MENUHELPER_H
#define	MENUHELPER_H

void menu(void);
uint8_t paging(uint8_t currentPage, const uint8_t lastPage);
void returnToPage(uint8_t page);

int8_t g_screenRefresh = 1;

#endif	/* MENUHELPER_H */

