/* 
 * File:   inputs.h
 * Author: User
 *
 * Created on 21. Januar 2020, 12:52
 */

#ifndef INPUTS_H
#define	INPUTS_H

enum e_direction {ROT_STOP, ROT_CW, ROT_CCW};
enum e_buttonPress {PB_NONE, PB_SHORT, PB_LONG, PB_ABORT, PB_WAIT};

volatile enum e_buttonPress pbState = PB_NONE;
volatile uint32_t userActivity = 0;

void rotISR(void);
void pushButtonISR(void);
enum e_buttonPress getPB(void);
enum e_direction getRotDir(void);
void spinInput(float *input, float min, float max, float step);
void spinInputI(int8_t *input, int8_t min, int8_t max, int8_t step);

#endif	/* INPUTS_H */

