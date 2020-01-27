/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef ERROR_H
#define	ERROR_H

enum e_errorcode {
    WARN_REMOVED, 
    WARN_SHORT, 
    WARN_VOLT_HIGH, 
    WARN_VOLT_LOW, 
    ERR_NUKED, 
    ERR_OVERCURRENT
};

void error(enum e_errorcode error);

#endif	

