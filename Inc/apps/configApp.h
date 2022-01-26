#ifndef _CONFIG_APP_H_
#define _CONFIG_APP_H_
#include <stdint.h>

#define CONFIG_ENCODERPOS_TIMEON 0
#define CONFIG_ENCODERPOS_TIMEOFF 1
#define CONFIG_ENCODERPOS_MODE 2
#define CONFIG_ENCODERPOS_TTARGET 3
#define CONFIG_ENCODERPOS_HEATER 4
#define CONFIG_ENCODERPOS_BACK 5
#define CONFIG_ENCODERPOS_OK 6
void createConfigApp(SubApplicationType* app,uint8_t index);
void configAppDisplay(void* data);
void configAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange);

void configDeselectEntity(uint8_t pos);
void configSelectEntity(uint8_t pos);
void timedelta(uint8_t* hours,uint8_t* min,int16_t delta);
#endif