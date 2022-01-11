/*
 * stringFunctions.h
 *
 *  Created on: 20.09.2021
 *      Author: philipp
 */
#include "systemChoice.h"
#ifndef STRINGFUNCTIONS_H_
#define STRINGFUNCTIONS_H_

#ifndef HARDWARE
#include <stdint.h>
#endif

void UInt8ToChar(uint8_t, char*);
void UInt16ToChar(uint16_t, char*);
void UInt32ToChar(uint32_t, char*);
uint8_t toUInt8(char*);
int16_t toInt16(char*);
uint32_t toUInt32(char*);

void UInt32ToHex(uint32_t,char*);

void fillWithLeadingZeros(uint8_t minlength,char * nr);
void toPercentChar(float,char*);


uint8_t startsWith(char*,char*);
void toUpper(char *,char);
void getBracketContent(const char*,char*);
void stripWhitespaces(char *);

/*
 * converts a string denoting an integer range as <lower>-<higher>, ex 12-17
 * into an array, ex 12, 13, 14, 15, 16, 17, returns the array length, first argument is the input string,
 * second argument is the pointer to the result array
 * */
uint8_t expandRange(char *,uint8_t**);


#endif /* STRINGFUNCTIONS_H_ */
