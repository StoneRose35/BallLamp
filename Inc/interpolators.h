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
} TasksType;

typedef TasksType* Tasks;

void initInterpolators(Tasks tasks);
uint8_t setLampInterpolator(Tasks tasks,uint8_t,uint8_t,uint8_t);
uint8_t setColorFramesInterpolation(Tasks tasks, uint8_t r,uint8_t g,uint8_t b,int16_t frames, uint8_t lamp_nr,uint8_t step, uint8_t interpolation);
uint8_t startInterpolators(Tasks tasks);
uint8_t stopInterpolators(Tasks tasks);

#endif /* INTERPOLATORS_H_ */
