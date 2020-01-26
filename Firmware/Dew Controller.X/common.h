/* 
 * File:   common.h
 * Author: André Küllenberg
 *
 * Created on 13. Januar 2020, 15:40
 */

#ifndef COMMON_H
#define	COMMON_H

#define _XTAL_FREQ 4000000
#define NUM_CHANNELS 4

#define len(array) sizeof(array)/sizeof(array[0])

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pins.h"
#include "interrupt.h"
#include "uart.h"


typedef struct {
	unsigned BAT_LOW:1;
	unsigned SENSOR_OK:1;
	unsigned AUX_SENSOR_OK:1;
} t_status;

enum e_channelMode {MANUAL, AUTO};
enum e_channelStatus {OFF, ON, OPEN, SHORTED, OVERCURRENT, REMOVED};

typedef struct {
	float cur;
	float Pmax;
	float Preq;
	float Patt;
	uint8_t DCreq;
	uint8_t DCatt;
	uint8_t lensDia;
	enum e_channelMode mode;
	enum e_channelStatus status;
} t_channelData;

typedef struct {
	float tempC;
	float relHum;
	float dewPointC;
	float sensorVersion;
	float tempAux;
	float voltage;
	float current; 
	float power;
	t_status status;
	t_channelData chData[NUM_CHANNELS];
} t_globalData;



#endif	/* COMMON_H */