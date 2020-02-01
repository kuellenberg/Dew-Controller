/* 
 * File:   system.h
 * Author: Andre
 *
 * Created on 
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

typedef union {
    struct {
        float lensDia[4];
        float dpOffset;
        float skyTemp;
        float fudgeFactor;
        float _dummy;
    };
    uint8_t raw[32];
} t_nvmData;

uint8_t checkSensor(void);
void checkChannelStatus(void);
void calcRequiredPower(void);
void channelThing(void);
uint8_t controller(void);
void systemCheck(void);
void getAnalogValues(void);
uint8_t storeNVM(void);
void readNVM(void); 


#endif