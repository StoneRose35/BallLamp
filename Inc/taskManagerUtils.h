/*
 * taskManagerUtils.h
 *
 *  Created on: 25.10.2021
 *      Author: philipp
 */

#ifndef INC_TASKMANAGERUTILS_H_
#define INC_TASKMANAGERUTILS_H_

#include "types.h"
#include "stringFunctions.h"
#include "system.h"

uint8_t tryToUInt8(char * str,uint8_t* has_errors_ptr);

int16_t tryToInt16(char * str,uint8_t* has_errors_ptr);

uint8_t checkLampRange(uint8_t,uint8_t* has_errors_ptr);

#endif /* INC_TASKMANAGERUTILS_H_ */
