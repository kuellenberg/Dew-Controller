/* 
 * File:   common.h
 * Author: André Küllenberg
 *
 * Created on 13. Januar 2020, 15:40
 */

#ifndef COMMON_H
#define	COMMON_H


#define _XTAL_FREQ 4000000

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
} t_globalData;

#endif	/* COMMON_H */

