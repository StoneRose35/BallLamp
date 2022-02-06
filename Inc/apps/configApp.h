#ifndef _CONFIG_APP_H_
#define _CONFIG_APP_H_
#include <stdint.h>

#define CONFIG_ENCODERPOS_TIMEON 0
#define CONFIG_ENCODERPOS_TIMEOFF 1
#define CONFIG_ENCODERPOS_MODE 2
#define CONFIG_ENCODERPOS_TTARGET 3
#define CONFIG_ENCODERPOS_HEATER 4
#define CONFIG_ENCODERPOS_NEXT_PAGE 5
#define CONFIG_ENCODERPOS_PREV_PAGE 6
#define CONFIG_ENCODERPOS_BACK 7
#define CONFIG_ENCODERPOS_OK 8

#define CONFIG_MODE_MANUAL 0 
#define CONFIG_MODE_AUTO 1
void createConfigApp(SubApplicationType* app,uint8_t index);
void configAppDisplay(void* data);
void configAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange);

void configDeselectEntity(uint8_t pos);
void configSelectEntity(uint8_t pos);
void timedelta(uint8_t* hours,uint8_t* min,int16_t delta);
void displayCurrentPage();

void increaseEncoderPos();
void decreaseEncoderPos();

// edit callbacks
void hOnChange(int16_t encoderIncr);
void hOffChange(int16_t encoderIncr);
void modeChange(int16_t encoderIncr);
void tTargetChange(int16_t encoderIncr);
void heaterChange(int16_t encoderIncr);

void lampChange(int16_t encoderIncr);
void cIntChange(int16_t encoderIncr);
void tLowerChange(int16_t encoderIncr);
void serviceIntervalChange(int16_t encoderIncr);


// display functions
void displayPage0();
void displayPage1();

void pageDecreaseChange();
void pageIncreaseChange();

void voidCallback(int16_t encoderIncr);
#endif