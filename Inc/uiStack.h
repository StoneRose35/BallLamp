#ifndef _UI_STACK_H_
#define _UI_STACK_H_
#include <stdint.h>

void uiStackTask(uint32_t task);
void setPagePtr(uint32_t ptr);
void display();
void initUiStack();

/**
 * @brief draws at 2 pixel tall bar spaced 16 pixel vertically apart
 * 
 * @param posx position in pixel
 * @param posy position in pixels
 * @param width width in pixels
 */
void drawSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width,uint8_t spacing);


void clearSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width,uint8_t spacing);

void drawSelectFrame(uint8_t posx,uint8_t posy);

void clearSelectFrame(uint8_t posx,uint8_t posy);

void drawArrows(uint8_t px, uint8_t py,uint8_t spacing);

void clearArrows(uint8_t px, uint8_t py,uint8_t spacing);

typedef struct 
{
    void(*encoderSwitchCallback)(int16_t encoderIncrement,int8_t switchChange); // switch change 1: pushed, -1:released
    void(*display)(void*);
    void(*loop)(void*);
    void* data;
} SubApplicationType;

typedef struct 
{
    uint8_t posx;
    uint8_t posy;
    uint8_t spacing;
    uint8_t width;

} SelectionPositionType;

#endif