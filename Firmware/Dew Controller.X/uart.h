/* 
 * File:   uart.h
 * Author: User
 *
 * Created on 21. Januar 2020, 13:00
 */

#ifndef UART_H
#define	UART_H

#define RX_BUF_LEN 20


typedef struct {
	uint8_t header;
	uint8_t version;
	uint8_t status;
	float tempC;
	float relHum;
	float dewPointC;
} t_dataPacket;

volatile uint8_t uartDataReadyFlag = 0;
volatile t_dataPacket dataPacket;

void uartReceiveISR(void);
void uartSendByte(char s);
void uartReset(void);


#endif	/* UART_H */

