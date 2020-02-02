/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef ERROR_H
#define	ERROR_H

enum e_errorcode {
	NO_ERROR,
    WARN_REMOVED, 
    WARN_SHORTED, 
	WARN_OVERCURRENT,
    WARN_HEATER_OVERCURRENT,
	WARN_VOLT_HIGH, 
    WARN_VOLT_LOW,
    WARN_SENSOR_TIMEOUT,
    WARN_SENSOR_CHECKSUM,
    ERR_NUKED, 
    ERR_OVERCURRENT,
    ERR_VOLT_CRIT,
	ERR_MENU
};

void error(enum e_errorcode code);
void viewErrorMessage(void);
enum e_errorcode getLastError(void);

#endif	

