#include "common.h"

//-----------------------------------------------------------------------------
// float to ascii
// Very crude implementation. Only works for small numbers.
// Result is truncated, not rounded.
//-----------------------------------------------------------------------------
void ftoa(char *str, float fValue, uint8_t width, uint8_t prec)
{
	uint8_t len = 1;
	uint8_t w = width;
	uint8_t pos = 0;
	uint8_t intValue = 0;
	uint8_t sign = 0;

	if (fValue < 0) {
		fValue *= -1;
		sign = 1;
		width--;
	}
	while(fValue >= 10.0) {
		fValue /= 10;
		len++;
	}
	w = len + prec;
	if (prec == 0)
		width++;
	while(--width > w)
		str[pos++] = ' ';
	if (sign)
		str[pos++] = '-';
	while(w--) {
		intValue = fValue;
		fValue = fValue * 10 - (10 * intValue);
		str[pos++] = intValue + '0';
		if (w && (w == prec))
			str[pos++] = '.';

	}
	str[pos] = '\0';
}

//-----------------------------------------------------------------------------
// uint8_t to ascii
//-----------------------------------------------------------------------------

void itoa(char *str, uint8_t value, uint8_t width)
{
	str[width] = '\0';
	while (value && width > 0) {
		str[--width] = (value % 10) + '0';
		value = value / 10;
	}
	while(width)
		str[--width] = ' ';
	
}


//-----------------------------------------------------------------------------
// Exponential moving average filter
//-----------------------------------------------------------------------------

uint16_t ema(uint16_t in, uint16_t average, uint32_t alpha)
{
	uint32_t tmp0;
	tmp0 = in * alpha + average * (65536 - alpha);
	return(tmp0 + 32768) / 65536;
}