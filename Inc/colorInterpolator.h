/*
 * colorInterpolator.h
 *
 *  Created on: 21.09.2021
 *      Author: philipp
 */

#include "types.h"
#include "system.h"

#ifndef COLORINTERPOLATOR_H_
#define COLORINTERPOLATOR_H_

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t interpolation; //  bit0: interpolation mode (set: linear, unset: constant)
	int16_t frames;
	int16_t deltar;
	int16_t deltag;
	int16_t deltab;

} ColorStepType;

typedef ColorStepType *ColorStep;

typedef struct {
	ColorStepType * steps;
	uint32_t stepProgressionCnt;
	uint8_t Nsteps;
	uint8_t stepCnt;
	uint8_t lamp_nr;
	uint8_t state; // bits 0-1: 0 stopped, 1 starting, 2 running, bit2: repeating if set
	uint16_t r_cur;
	uint16_t b_cur;
	uint16_t g_cur;
} TaskType;

typedef TaskType *Task;



void initTask(Task t,uint8_t nsteps);
void setColor(Task t,uint8_t r,uint8_t g, uint8_t b,uint8_t idx);
void setFrames(Task t,uint32_t nframes,uint8_t idx);
void setReady(Task t);
void updateTask(Task t,RGBStream * lampdata);


#endif /* COLORINTERPOLATOR_H_ */
