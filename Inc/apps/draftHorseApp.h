#ifndef _DRAFT_HORSE_APP_H_
#define _DRAFT_HORSE_APP_H_
#include <stdint.h>
#include "uiStack.h"

void createDraftHorseApp(SubApplicationType* app,uint8_t index);
void draftHorseDisplay(void* data);
void draftHorseAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange);

#endif