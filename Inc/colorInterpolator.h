/*
 * colorInterpolator.h
 *
 *  Created on: 21.09.2021
 *      Author: philipp
 */




#ifndef COLORINTERPOLATOR_H_
#define COLORINTERPOLATOR_H_

#include "types.h"
#include "system.h"

#define STATE_STOPPED 0
#define STATE_STARTING 1
#define STATE_RUNNING 2
#define STATE_REPEATING 2
#define INTERPOLATION_CONSTANT 0
#define INTERPOLATION_LINEAR 1
#define MODE_ONE_SHOT 0
#define MODE_REPEATING 1

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


/**
 * initializes a Task, reserves space for the given number of steps
 */
void initTask(Task t,uint8_t nsteps,uint8_t lampnr);

/**
 * resets the conters to zeros, frees the space of the steps array
 * has to be called prior th defining a new task at a certain position in the tasks array
 */
void destroyTask(Task t);

/**
 * sets the color present at the beginning of the step, this colors is either faded to the next step's color
 * if interpolation is linear or kept until the next step
 */
void setColor(Task t,uint8_t r,uint8_t g, uint8_t b,uint8_t idx);

/**
 * sets the number of frames a given steps should last, 30 frames is 1s
 */
void setFrames(Task t,int16_t nframes,uint8_t idx);

/**
 * make the interpolator run on the next task loop iteration
 */
void start(Task t);

/**
 * stops the interpolator, the counters are reset
 */
void stop(Task t);

/**
 * pauses the interpolator, the counters are not reset
 */
void pause(Task t);

/**
 * resumes the interpolator, essentially restarts it without resetting the counters
 */
void resume(Task t);

/**
 * sets the lamp which is affected by the interpolator
 */
void setLampNr(Task,uint8_t);

/**
 * called by the system at each frame timedelta interrupt to update the current color of the lamp controlled by the interpolator
 */
void updateTask(Task t,RGBStream * lampdata);


/**
 * gets the current Task Progression as a number from 0 to 1
 */
float getProgression(Task t);

/**
 * returns the size in bytes
 */
uint32_t getSize(Task t);


#endif /* COLORINTERPOLATOR_H_ */
