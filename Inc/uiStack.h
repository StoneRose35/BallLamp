#ifndef _UI_STACK_H_
#define _UI_STACK_H_
#include <stdint.h>

void uiStackTask(uint32_t task);
void setPagePtr(uint32_t ptr);
void display();
void initUiStack();

typedef struct 
{
    void(*encoderSwitchCallback)(int16_t encoderIncrement,int8_t switchChange); // switch change 1: pushed, -1:released
    void(*display)(void*);
    void(*loop)(void*);
    void* data;
} SubApplicationType;

#endif