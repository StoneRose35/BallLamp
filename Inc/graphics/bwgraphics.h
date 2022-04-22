#ifndef _BWGRAPHICS_H_
#define _BWGRAPHICS_H_
#include <stdint.h>

typedef struct BwImageStruct
{
	uint8_t * data; // data is arrange y axis first, starting from top left
	uint8_t sx; // size in pixels
	uint8_t sy; // size in pixels
} BwImageType;

void drawLine(float spx,float spy,float epx, float epy,BwImageType* img);
void setPixel(int32_t px,int32_t py,BwImageType*img);
void clearPixel(int32_t px,int32_t py,BwImageType*img);

void drawOval(float ax,float ay,float cx,float cy,BwImageType*img);
void clearOval(float ax,float ay,float cx,float cy,BwImageType*img);
void clearSquare(float spx, float spy,float epx, float epy,BwImageType* img);
void drawSquare(float spx, float spy,float epx, float epy,BwImageType* img);

#endif