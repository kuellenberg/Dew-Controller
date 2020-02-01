
#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"


#define WRITE_FLASH_BLOCKSIZE    32
#define ERASE_FLASH_BLOCKSIZE    32
#define END_FLASH                0x4000

/**
  Section: Flash Module APIs
*/

/**

  @Example
    <code>
    uint16_t    readWord;
    uint16_t    flashAddr = 0x01C0;

    readWord = FLASH_ReadWord(flashAddr);
    </code>
*/
uint16_t FLASH_ReadWord(uint16_t flashAddr);

/**
  @Example
    <code>
    uint16_t    writeData = 0x15AA;
    uint16_t    flashAddr = 0x01C0;
    uint16_t    Buf[ERASE_FLASH_BLOCKSIZE];

    FLASH_WriteWord(flashAddr, Buf, writeData);
    </code>
*/
void FLASH_WriteWord(uint16_t flashAddr, uint16_t *ramBuf, uint16_t word);

/**
  @Example
    <code>
    #define FLASH_ROW_ADDRESS     0x01C0

    uint16_t wrBlockData[] =
    {
        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000D, 0x000F,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
        0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F
    }

    // write to Flash memory block
    FLASH_WriteBlock((uint16_t)FLASH_ROW_ADDRESS, (uint16_t*)wrBlockData);
    </code>
*/
int8_t FLASH_WriteBlock(uint16_t writeAddr, uint16_t *flashWordArray);

/**

  @Example
    <code>
    uint16_t    flashBlockStartAddr = 0x01C0;

    FLASH_EraseBlock(flashBlockStartAddr);
    </code>
*/
void FLASH_EraseBlock(uint16_t startAddr);



void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData);

/**


  @Example
    <code>
    uint16_t dataeeAddr = 0x3F80;
    uint8_t readData;

    readData = DATAEE_ReadByte(dataeeAddr);
    </code>
*/
uint8_t DATAEE_ReadByte(uint16_t bAdd);


#endif // MEMORY_H
