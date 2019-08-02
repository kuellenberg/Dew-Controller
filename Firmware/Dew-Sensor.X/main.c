/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.76
        Device            :  PIC16F15323
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
//#include <math.h>


#define SI7006_RETRY_MAX  3  // define the retry count
#define SI7006_ADDRESS    0x40 // slave device address

/*
typedef struct {
	float temp_C;
	float RH;
	float td;
	uint8_t status;
} DATA_STRUCT;
*/
typedef struct {
	uint16_t temp_C_code;
	uint16_t RH_code;
		uint8_t status;
} DATA_STRUCT;

typedef union {
    DATA_STRUCT s;
    uint8_t raw[sizeof(DATA_STRUCT)];
} DATA_OBJ;


/*
                         Main application
 */
void main(void)
{
    uint8_t i;
	uint8_t readBuffer[2];
    uint8_t writeBuffer[2];
	uint16_t rh_code, temp_code;
	//float H;
    I2C1_MESSAGE_STATUS status = 0;
    DATA_OBJ data;
	
    // initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    while (1)
    {
		// Measure rel. humidity, no hold master
        writeBuffer[0] = 0xf5;
        I2C1_MasterWrite(writeBuffer,1, 0x40, &status);
        __delay_ms(100);
        I2C1_MasterRead(readBuffer, 2, 0x40, &status);
        __delay_ms(100);
        rh_code = readBuffer[0] | readBuffer[1] << 8;
        
//		// Read temperature from previous RH measurement
//		writeBuffer[0] = 0xe0;
//        I2C1_MasterWrite(writeBuffer,1, 0x40, &status);
//        __delay_ms(100);
//        I2C1_MasterRead(readBuffer, 2, 0x40, &status);
//        __delay_ms(100);
//        temp_code = readBuffer[0] | readBuffer[1] << 8;
        
        data.s.RH_code = rh_code;
        data.s.temp_C_code = temp_code;
        
		// convert rh and temp codes
		//data.s.RH = (125.0 * rh_code) / 65536 - 6;
		//data.s.temp_C = (175.72 * temp_code) / 65536 - 46.85;
		
		// calculate dew point temperature
		//H = (log10(data.s.RH) - 2.0) / 0.4343 + (17.62 * data.s.temp_C) / (243.12 + data.s.temp_C);
		//data.s.td = 243.12 * H / (17.62 - H);
	
//        for (i=0; i < sizeof(DATA_OBJ); i++) {
//            EUSART1_Write(data.raw[i]);
//        }
        	
		__delay_ms(1000);
    }
}
/**
 End of File
*/