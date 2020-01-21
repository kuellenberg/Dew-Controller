#include "common.h"
#include "interrupt.h"
#include "inputs.h"

#define TIME_MAX 1000000000UL

volatile uint8_t tick10ms = 0;
volatile uint32_t tick100ms = 0;

//-----------------------------------------------------------------------------
// Returns 100ms time tick
//-----------------------------------------------------------------------------
uint32_t timeNow(void)
{
	return tick100ms;
}

//-----------------------------------------------------------------------------
// Returns number of 100ms ticks since time 'since'
//-----------------------------------------------------------------------------
uint32_t timeSince(uint32_t since)
{
	uint32_t now = timeNow();
	if (now >= since)
		return (now - since);
	// Rollover
	return (now + (1 + TIME_MAX - since));
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------
uint8_t get10msTick(void)
{
	return tick10ms;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------
void reset10msTick(void) {
	tick10ms = 0;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routine
//-----------------------------------------------------------------------------
void __interrupt() ISR(void)
{
	if (PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1) {
		// Timer 0 ISR
		// Used for push button timing
		tick10ms++;
		TMR0 = TMR0_PRELOAD;
		PIR0bits.TMR0IF = 0;
	} else if (PIE0bits.IOCIE == 1 && PIR0bits.IOCIF == 1) {
		// Interrupt on change ISRs
		if (IOCAFbits.IOCAF7 == 1) {
			pushButtonISR();
			IOCAFbits.IOCAF7 = 0;
		}
		if (IOCAFbits.IOCAF4 == 1) {
			rotISR();
			IOCAFbits.IOCAF4 = 0;
		}
		if (IOCAFbits.IOCAF5 == 1) {
			rotISR();
			IOCAFbits.IOCAF5 = 0;
		}
		PIR0bits.IOCIF = 0;
	} else if (INTCONbits.PEIE == 1) {
		if (PIE4bits.TMR1IE == 1 && PIR4bits.TMR1IF == 1) {
			// Timer 1 ISR
			// 0.1s System tick
			if (tick100ms++ >= TIME_MAX)
				tick100ms = 0;
			TMR1 = TMR1_PRELOAD;
			PIR4bits.TMR1IF = 0;
		} else if (PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1) {
			uartReceiveISR();
			PIR3bits.RC1IF = 0;
		}
	}
}