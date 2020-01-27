/* 
 * File:   menu.h
 * Author: User
 *
 * Created on 21. Januar 2020, 15:56
 */

#ifndef MENU_H
#define	MENU_H

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
uint8_t statusView(t_globalData *data);
uint8_t channelView(t_globalData *data);
uint8_t channelSetup(t_globalData *data);
uint8_t setOutputPower(t_globalData *data);
uint8_t setLensDia(t_globalData *data);
uint8_t setup(t_globalData *data);
uint8_t setDPOffset(t_globalData *data);
uint8_t setSkyTemp(t_globalData *data);
uint8_t setFudgeFactor(t_globalData *data);

#endif	/* MENU_H */

