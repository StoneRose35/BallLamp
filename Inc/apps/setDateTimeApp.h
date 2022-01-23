#ifndef _SET_DATE_TIME_APP_H_
#define _SET_DATE_TIME_APP_H_
#include <stdint.h>

#define SETDT_ENCODERPOS_DAY 0
#define SETDT_ENCODERPOS_MONTH 1
#define SETDT_ENCODERPOS_YEAR 2
#define SETDT_ENCODERPOS_HOUR 3
#define SETDT_ENCODERPOS_MIN 4
#define SETDT_ENCODERPOS_SEC 5
#define SETDT_ENCODERPOS_BACK 6
#define SETDT_ENCODERPOS_OK 7

void createSetDateTimeApp(SubApplicationType* app,uint8_t index);
void setDateTimeAppDisplay(void* data);
void setDateTimeAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange);
void clearSelectFrame(uint8_t posx,uint8_t posy);
void drawSelectFrame(uint8_t posx,uint8_t posy);
void clearSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width);
void drawSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width);

void selectEntity(uint8_t pos);
void deselectEntity(uint8_t pos);

void drawArrows(uint8_t px, uint8_t py,uint8_t spacing);
void clearArrows(uint8_t px, uint8_t py,uint8_t spacing);

#endif