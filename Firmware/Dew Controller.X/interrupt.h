#ifndef INTERRUPT_H
#define INTERRUPT_H


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TMR0_PRELOAD 178
#define TMR1_PRELOAD 53035

#define timeNow() tick100ms

volatile uint8_t tick10ms = 0;
volatile uint32_t tick100ms = 0;

uint32_t timeSince(uint32_t since);
void __interrupt() ISR(void);

#endif