/*
 * consoleHandler.h
 *
 *  Created on: 15.09.2021
 *      Author: philipp
 */

#ifndef CONSOLEHANDLER_H_
#define CONSOLEHANDLER_H_

#define COMMAND_BUFFER_SIZE 256
#define OUT_BUFFER_SIZE 64

char* onCharacterReception(uint8_t);
void clearCommandBuffer();
void clearOutBuffer();


#endif /* CONSOLEHANDLER_H_ */
