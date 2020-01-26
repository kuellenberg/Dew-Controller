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

#define ON 1
#define OFF 0

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

enum e_channelMode {MODE_MANUAL, MODE_AUTO};
enum e_channelStatus {CH_DISABLED, CH_ENABLED, CH_OPEN, CH_SHORTED, CH_OVERCURRENT, CH_REMOVED};

typedef struct {
	float current;
	float Pmax;
	float Preq;
	float Patt;
	uint8_t DCreq;
	uint8_t DCatt;
	float lensDia;
	float dt;	// approx. heater temp. required
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
	float dpOffset;
	float skyTemp;
	float fudgeFactor;
	t_status status;
	t_channelData chData[NUM_CHANNELS];
} t_globalData;



#endif	/* COMMON_H */