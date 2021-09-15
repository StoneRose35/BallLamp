/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */


#include "led_timer.h"
#include "system.h"
#include "blink.h"
#include "systemClock.h"
#include "uart.h"


RGBStream framedata[N_LAMPS];
RGBStream * frame = framedata;
uint32_t clr_cnt = 0;
uint32_t bit_cnt = 0;

uint8_t rawdata[N_LAMPS*24+1];
uint8_t* rawdata_ptr = rawdata;


/*
 * updates the color along a hue shift with the phase going from 0 to 1535
 * */
void colorUpdate(RGB * color,uint32_t phase)
{
	if (phase < 0x100)
	{
		color->r = 0xFF;
		color->g = phase & 0xFF;
		color->b = 0x00;
	}
	else if (phase < 0x200)
	{
		color->r = 0x1FF - phase;
		color->g = 0xFF;
		color->b = 0x00;
	}
	else if (phase < 0x300)
	{
		color->r = 0x00;
		color->g = 0xFF;
		color->b = phase - 0x200;
	}
	else if (phase < 0x400)
	{
		color->r = 0x00;
		color->g = 0x3FF - phase;
		color->b = 0xFF;
	}
	else if (phase < 0x500)
	{
		color->r = phase - 0x400;
		color->g = 0x00;
		color->b = 0xFF;
	}
	else
	{
		color->r = 0xFF;
		color->g = 0x00;
		color->b = 0x5FF - phase;
	}
}

void setColor(RGB * color,uint8_t nr)
{
	(frame + nr)->rgb.r = color->r;
	(frame + nr)->rgb.g = color->g;
	(frame + nr)->rgb.b = color->b;
}

int main(void)
{
	uint32_t phasecnt = 0;
    setupClock();

	initTimer();
	initUart();

	colorUpdate(&(frame->rgb),phasecnt);
	decompressRgbArray(frame,N_LAMPS);


	uint32_t dummycnt = 0;
	printf("BallLamp v0.1 running\r\n");

    /* Loop forever */
	for(;;)
	{

		if ((TIM2->CR1 & 1) != 1) // if timer 2 is not running, i.e. data transfer is over
		{
			sendToLed(); // non-blocking, returns long before the neopixel clock pulses have been sent
			dummycnt++;
		}
		if (TIM2->ARR > WS2818_CNT && dummycnt > 0) // is in wait state after after the data transfer
		{
			decompressRgbArray(frame,N_LAMPS);
			dummycnt = 0;

			// this would be a "hue shift" program for the first lamp
			//colorUpdate(&(frame->rgb),phasecnt);
			//phasecnt += PHASE_INC;
			//if (phasecnt>0x5FF)
			//{
			//	phasecnt=0;
			//}
		}
	}
}
