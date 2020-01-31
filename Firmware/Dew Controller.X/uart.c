#include "common.h"

volatile uint8_t rxFErrCount = 0;
volatile uint8_t rxOErrCount = 0;
volatile uint8_t rxCount = 0;
volatile uint8_t dataReadyFlag = 0;
static t_dataPacket dataPacket;


//-----------------------------------------------------------------------------
// Returns True after data packet has been received
//-----------------------------------------------------------------------------
uint8_t uartIsDataReady(void)
{
	uint8_t ret = dataReadyFlag;
	dataReadyFlag = 0;
	return ret;
}
//-----------------------------------------------------------------------------
// Returns pointer to dataPacket
//-----------------------------------------------------------------------------
t_dataPacket *getDataPacket(void)
{
	return &dataPacket;
}
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
	static char buffer[RX_BUF_LEN];
	
	static uint8_t checksum = 0;

	if (RC1STAbits.OERR) // Receiver buffer overrun error
	{
		RC1STAbits.CREN = 0;
		RC1STAbits.CREN = 1;
		rxOErrCount++;
	}
	if (RC1STAbits.FERR) // Framing error
	{
		RC1STAbits.SPEN = 0;
		RC1STAbits.SPEN = 1;
		rxFErrCount++;
	}

	// Store incoming bytes in temporary buffer
	if (rxCount < sizeof(dataPacket)) {
		buffer[rxCount] = RC1REG;
		checksum ^= buffer[rxCount];
		rxCount++;
	} else {
		// Last byte is checksum
		if (RC1REG == checksum) {
			// set data ready flag and copy buffer to data structure
			dataReadyFlag = 1;
			strncpy((char *) &dataPacket, buffer, sizeof(dataPacket));
		}
		checksum = 0;
		rxCount = 0;
	}
}

//-----------------------------------------------------------------------------
// Reset UART module
//-----------------------------------------------------------------------------
void uartReset(void)
{
	uint8_t dump;
	dump = RC1REG;
	RC1STAbits.CREN = 0;
	RC1STAbits.CREN = 1;
	RC1STAbits.SPEN = 0;
	RC1STAbits.SPEN = 1;
	rxCount = 0;
}