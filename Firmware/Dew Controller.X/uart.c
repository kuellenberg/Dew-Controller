#include "common.h"
#include "uart.h"
#include "interrupt.h"

#define RX_BUF_LEN 20

//volatile uint8_t rxFErrCount = 0;
//volatile uint8_t rxOErrCount = 0;
volatile uint8_t rxCount = 0;
static char buffer[RX_BUF_LEN];
static uint8_t checksum = 0;

//-----------------------------------------------------------------------------
// Transmit character string over UART
//-----------------------------------------------------------------------------
void uartSendByte(char s)
{
	TX1REG = s;
	NOP();
	while (!PIR3bits.TX1IF);
}

//-----------------------------------------------------------------------------
// EUSART Receive Interrupt
//-----------------------------------------------------------------------------
void uartReceiveISR(void)
{
	
	if (RC1STAbits.OERR) // Receiver buffer overrun error
	{
		RC1STAbits.CREN = 0;
		RC1STAbits.CREN = 1;
		//rxOErrCount++;
	}
	if (RC1STAbits.FERR) // Framing error
	{
		RC1STAbits.SPEN = 0;
		RC1STAbits.SPEN = 1;
		//rxFErrCount++;
	}

	// Store incoming bytes in temporary buffer
	if (rxCount < sizeof(dataPacket)) {
		buffer[rxCount] = RC1REG;
		checksum ^= buffer[rxCount];
		rxCount++;
	} else {
		// Last byte is checksum
		if (RC1REG != checksum)
			dataPacket.status = 0;
		// set data ready flag and copy buffer to data structure
		uartDataReadyFlag = 1;
		memcpy((void *) &dataPacket,(void *) buffer, sizeof(dataPacket));
		checksum = 0;
		rxCount = 0;
	}
}

//-----------------------------------------------------------------------------
// Reset UART module
//-----------------------------------------------------------------------------
void uartReset(void)
{
	RC1STAbits.CREN = 0;
	RC1STAbits.CREN = 1;
	RC1STAbits.SPEN = 0;
	RC1STAbits.SPEN = 1;
	rxCount = 0;
}