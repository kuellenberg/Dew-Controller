#ifndef INTERRUPT_H
#define INTERRUPT_H


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define TMR0_PRELOAD 178
#define TMR1_PRELOAD 53035

void reset10msTick(void);
uint8_t get10msTick(void);
uint32_t timeNow(void);
uint32_t timeSince(uint32_t since);
void __interrupt() ISR(void);

#endif