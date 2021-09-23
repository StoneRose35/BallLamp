/*
 * stringFunctions.h
 *
 *  Created on: 20.09.2021
 *      Author: philipp
 */

#ifndef STRINGFUNCTIONS_H_
#define STRINGFUNCTIONS_H_

void UInt8ToChar(uint8_t, char*);
void UInt16ToChar(uint16_t, char*);
void UInt32ToChar(uint32_t, char*);
uint8_t startsWith(const char*,const char*);
uint8_t toInt(char*);
void getBracketContent(const char*,char*);
void split(const char* input,const char sep,char*out);


#endif /* STRINGFUNCTIONS_H_ */
