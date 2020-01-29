#include "io.h"

void setLoadSwitch(uint8_t state)
{
	PEN = state;
}

void setChannelSwitch(uint8_t channel, uint8_t state)
{
	switch(channel) {
	case 0:
		SW_CH0 = state;
		break;
	case 1:
		SW_CH1 = state;
		break;
	case 2:
		SW_CH2 = state;
		break;
	case 3:
		SW_CH3 = state;
		break;
	default:
		SW_CH0 = state;
		SW_CH1 = state;
		SW_CH2 = state;
		SW_CH3 = state;
		break;
	}
}

//-----------------------------------------------------------------------------
// Start ADC conversion and return result
//-----------------------------------------------------------------------------

uint16_t getAnalogValue(uint8_t channel)
{
	ADCON0bits.CHS = channel;
	__delay_us(5);
	ADCON0bits.GO = 1;
	while (ADCON0bits.GO);
	return(uint16_t) ((ADRESH << 8) + ADRESL);
}