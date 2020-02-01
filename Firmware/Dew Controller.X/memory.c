#include <xc.h>
#include "memory.h"

/**
  Section: Flash Module APIs
 */

uint16_t FLASH_ReadWord(uint16_t flashAddr)
{
	uint8_t GIEBitValue = INTCONbits.GIE; // Save interrupt enable

	INTCONbits.GIE = 0; // Disable interrupts
	NVMADRL = (flashAddr & 0x00FF);
	NVMADRH = ((flashAddr & 0xFF00) >> 8);

	NVMCON1bits.NVMREGS = 0; // Deselect Configuration space
	NVMCON1bits.RD = 1; // Initiate Read
	NOP();
	NOP();
	INTCONbits.GIE = GIEBitValue; // Restore interrupt enable

	return((uint16_t) ((NVMDATH << 8) | NVMDATL));
}

void FLASH_WriteWord(uint16_t flashAddr, uint16_t *ramBuf, uint16_t word)
{
	uint16_t blockStartAddr = (uint16_t) (flashAddr & ((END_FLASH - 1) ^ (ERASE_FLASH_BLOCKSIZE - 1)));
	uint8_t offset = (uint8_t) (flashAddr & (ERASE_FLASH_BLOCKSIZE - 1));
	uint8_t i;

	// Entire row will be erased, read and save the existing data
	for (i = 0; i < ERASE_FLASH_BLOCKSIZE; i++) {
		ramBuf[i] = FLASH_ReadWord((blockStartAddr + i));
	}

	// Write at offset
	ramBuf[offset] = word;

	// Writes ramBuf to current block
	FLASH_WriteBlock(blockStartAddr, ramBuf);
}

int8_t FLASH_WriteBlock(uint16_t writeAddr, uint16_t *flashWordArray)
{
	uint16_t blockStartAddr = (uint16_t) (writeAddr & ((END_FLASH - 1) ^ (ERASE_FLASH_BLOCKSIZE - 1)));
	uint8_t GIEBitValue = INTCONbits.GIE; // Save interrupt enable
	uint8_t i;


	// Flash write must start at the beginning of a row
	if (writeAddr != blockStartAddr) {
		return -1;
	}

	INTCONbits.GIE = 0; // Disable interrupts

	// Block erase sequence
	FLASH_EraseBlock(writeAddr);

	// Block write sequence
	NVMCON1bits.NVMREGS = 0; // Deselect Configuration space
	NVMCON1bits.WREN = 1; // Enable wrties
	NVMCON1bits.LWLO = 1; // Only load write latches

	for (i = 0; i < WRITE_FLASH_BLOCKSIZE; i++) {
		// Load lower 8 bits of write address
		NVMADRL = (writeAddr & 0xFF);
		// Load upper 6 bits of write address
		NVMADRH = ((writeAddr & 0xFF00) >> 8);

		// Load data in current address
		NVMDATL = flashWordArray[i];
		NVMDATH = ((flashWordArray[i] & 0xFF00) >> 8);

		if (i == (WRITE_FLASH_BLOCKSIZE - 1)) {
			// Start Flash program memory write
			NVMCON1bits.LWLO = 0;
		}

		NVMCON2 = 0x55;
		NVMCON2 = 0xAA;
		NVMCON1bits.WR = 1;
		NOP();
		NOP();

		writeAddr++;
	}

	NVMCON1bits.WREN = 0; // Disable writes
	INTCONbits.GIE = GIEBitValue; // Restore interrupt enable

	return 0;
}

void FLASH_EraseBlock(uint16_t startAddr)
{
	uint8_t GIEBitValue = INTCONbits.GIE; // Save interrupt enable


	INTCONbits.GIE = 0; // Disable interrupts
	// Load lower 8 bits of erase address boundary
	NVMADRL = (startAddr & 0xFF);
	// Load upper 6 bits of erase address boundary
	NVMADRH = ((startAddr & 0xFF00) >> 8);

	// Block erase sequence
	NVMCON1bits.NVMREGS = 0; // Deselect Configuration space
	NVMCON1bits.FREE = 1; // Specify an erase operation
	NVMCON1bits.WREN = 1; // Allows erase cycles

	// Start of required sequence to initiate erase
	NVMCON2 = 0x55;
	NVMCON2 = 0xAA;
	NVMCON1bits.WR = 1; // Set WR bit to begin erase
	NOP();
	NOP();

	NVMCON1bits.WREN = 0; // Disable writes
	INTCONbits.GIE = GIEBitValue; // Restore interrupt enable
}

/**
  Section: Data EEPROM Module APIs
 */

/**
@Preconditions
    This device dont have a EEPROM.
	SAF area can be emaluted as an EEPROM.
	SAFEN config bit in mcc.c needs to be set to ON before calling this function.
	The last 128 bytes of program memory is allocated for SAF when the SAFEN bit is ON.
 **/

void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData)
{
	uint8_t GIEBitValue = INTCONbits.GIE;

	NVMADRH = ((bAdd >> 8) & 0xFF);
	NVMADRL = (bAdd & 0xFF);
	NVMDATL = bData;
	NVMCON1bits.NVMREGS = 0;
	NVMCON1bits.WREN = 1;
	INTCONbits.GIE = 0; // Disable interrupts
	NVMCON2 = 0x55;
	NVMCON2 = 0xAA;
	NVMCON1bits.WR = 1;
	// Wait for write to complete
	while (NVMCON1bits.WR) {
	}

	NVMCON1bits.WREN = 0;
	INTCONbits.GIE = GIEBitValue; // restore interrupt enable
}

uint8_t DATAEE_ReadByte(uint16_t bAdd)
{
	uint8_t GIEBitValue = INTCONbits.GIE; // Save interrupt enable
	INTCONbits.GIE = 0;
	NVMADRH = ((bAdd >> 8) & 0xFF);
	NVMADRL = (bAdd & 0xFF);
	NVMCON1bits.NVMREGS = 0;
	NVMCON1bits.RD = 1;
	NOP(); // NOPs may be required for latency at high frequencies
	NOP();
	__delay_ms(10);
	INTCONbits.GIE = GIEBitValue;
	return(NVMDATL);
}


/**
 End of File
 */
