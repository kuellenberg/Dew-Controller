/* 
 * File:   system.h
 * Author: Andre
 *
 * Created on 
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

uint8_t checkSensor(t_globalData *data);
uint8_t checkChannelStatus(t_globalData *data);
void calcRequiredPower(t_globalData *data);
void channelThing(t_globalData *data);
uint8_t controller(void);
void systemCheck(t_globalData *data);
void getAnalogValues(t_globalData *data);

#endif