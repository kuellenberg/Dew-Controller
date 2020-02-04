/* 
 * File:   common.h
 * Author: André Küllenberg
 *
 * Created on 13. Januar 2020, 15:40
 */

#ifndef COMMON_H
#define	COMMON_H

#define _XTAL_FREQ 4000000UL

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define VERSION 1
#define NUM_CHANNELS 4	// number of heater channels
#define COLUMNS 12		// columns on display
#define DISPLAY_TIMEOUT 1200

// EMA function needs scaled alpha value
#define ALPHA(x) ( (uint32_t)(x * 65535) ) 
// Returns length of an array
#define len(array) sizeof(array)/sizeof(array[0])

#define MIN(a, b) ((a) < (b) ? (a) : (b)) 

// Status bits 
typedef struct {
	unsigned BAT_LOW:1;
    unsigned BAT_HIGH:1;
    unsigned OVERCURRENT:1;
	unsigned SENSOR_OK:1;
	unsigned AUX_SENSOR_OK:1;
} t_status;

// Channel status
enum e_channelMode {MODE_MANUAL, MODE_AUTO};
enum e_channelStatus {
	CH_DISABLED, 
	CH_ENABLED, 
	CH_OPEN, 
	CH_SHORTED, 
	CH_OVERCURRENT, 
	CH_UNCHECKED
};

// Channel data
typedef struct {
	float current;
	float Pmax;
	float Preq;
	float Patt;
    float Pset;
	uint8_t DCreq;
	float lensDia;
	float dt;	// approx. heater temp. required
	enum e_channelMode mode;
	enum e_channelStatus status;
} t_heater;

// Global data structure
typedef struct {
	float tempC;
	float relHum;
	float dewPointC;
	uint8_t sensorVersion;
	float tempAux;
	float voltage;
	float current; 
	float power;
	float dpOffset;
	float skyTemp;
	float fudgeFactor;
	t_status status;
	t_heater heater[NUM_CHANNELS];
} t_globalData;


//-----------------------------------------------------------------------------
// Global data structure 
//-----------------------------------------------------------------------------
t_globalData data;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void ftoa(char *str, float fValue, uint8_t width, uint8_t prec);
void itoa(char *str, uint8_t value, uint8_t width);
uint16_t ema(uint16_t in, uint16_t average, uint32_t alpha);

#endif	/* COMMON_H */