/*
 * interpolators.h
 *
 *  Created on: 21.10.2021
 *      Author: philipp
 */

#ifndef INTERPOLATORS_H_
#define INTERPOLATORS_H_

#define ERR_INVALID_STEP 128

#include "colorInterpolator.h"

typedef struct {
	TaskType* taskArray;
	uint8_t taskArrayLength;
	char name[32];
} TasksType;

typedef TasksType* Tasks;

void initInterpolators(Tasks tasks);
uint8_t getLampIndex(Tasks,uint8_t);
uint8_t setLampInterpolator(Tasks tasks,uint8_t,uint8_t,uint8_t);
uint8_t setColorFramesInterpolation(Tasks, uint8_t,uint8_t,uint8_t,int16_t, uint8_t,uint8_t, uint8_t);
uint8_t startInterpolators(Tasks tasks);
uint8_t stopInterpolators(Tasks tasks);


#endif /* INTERPOLATORS_H_ */
