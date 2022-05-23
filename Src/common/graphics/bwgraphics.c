#include "graphics/bwgraphics.h"
#include "stdlib.h"
#ifdef RP2040_FEATHER
#include "romfunc.h"
#else
#include "math.h"
float fsqrt(float a)
{
    return sqrtf(a);
}

int32_t float2int(float a)
{
    return (int32_t)a;
}

float int2float(int32_t a)
{
    return (float)a;
}
#endif


void drawLine(float spx,float spy,float epx, float epy,BwImageBufferType* img)
{
	float l,fc;
	int32_t il;
    int32_t x,y;
	l = fsqrt((epy-spy)*(epy-spy) + (epx-spx)*(epx-spx));
	il = float2int(l);
	for(uint32_t c=0;c<il;c++)
	{
		fc = int2float(c);
		x = float2int(spx + (fc/l)*(epx-spx));
		y = float2int(spy + (fc/l)*(epy-spy));
		setPixel(x,y,img);
	}
}

void drawOval(float ax,float ay,float cx,float cy,BwImageBufferType*img)
{
	float fix,fiy,dr;
	for(int32_t ix=0;ix<img->sx;ix++)
	{
		for(int32_t iy=0;iy<img->sy;iy++)
		{
			fix = int2float(ix);
			fiy = int2float(iy);
			dr = (fix-cx)*(fix-cx)/ax/ax + (fiy-cy)*(fiy-cy)/ay/ay; 
			if (dr <= 1.0f)
			{
				setPixel(ix,iy,img);
			}
		}
	}
}

void clearOval(float ax,float ay,float cx,float cy,BwImageBufferType*img)
{
	float fix,fiy,dr;
	for(int32_t ix=0;ix<img->sx;ix++)
	{
		for(int32_t iy=0;iy<img->sy;iy++)
		{
			fix = int2float(ix);
			fiy = int2float(iy);
			dr = (fix-ix)*(fix-ix)/ax/ax + (fiy-iy)*(fiy-iy)/ay/ay; 
			if (dr <= 1.0f)
			{
				clearPixel(ix,iy,img);
			}
		}
	}
}

void clearSquare(float spx, float spy,float epx, float epy,BwImageBufferType* img)
{
	uint32_t dx,dy;
	dx=(uint32_t)float2int(epx-spx);
	dy=(uint32_t)float2int(epy-spy);
	int32_t px,py;
	px=float2int(spx);
	py=float2int(spy);
	for (int32_t cx = 0;cx<dx;cx++)
	{
		for(int32_t cy=0;cy<dy;cy++)
		{
			clearPixel(px + cx,py + cy,img);
		}
	}
}

void drawSquare(float spx, float spy,float epx, float epy,BwImageBufferType* img)
{
	uint32_t dx,dy;
	dx=(uint32_t)float2int(epx-spx);
	dy=(uint32_t)float2int(epy-spy);
	int32_t px,py;
	px=float2int(spx);
	py=float2int(spy);
	for (uint8_t cx = 0;cx<dx;cx++)
	{
		for(uint8_t cy=0;cy<dy;cy++)
		{
			setPixel(px + cx,py + cy,img);
		}
	}
}

void setPixel(int32_t px,int32_t py,BwImageBufferType*img)
{
	int32_t bitindex = px*(img->sy-1) + py;
	int32_t pageIdx = bitindex >> 3;
	*(img->data + pageIdx) |= (1 << (bitindex & 0x7));
}

void clearPixel(int32_t px,int32_t py,BwImageBufferType*img)
{
	int32_t bitindex = px*(img->sy-1) + py;
	int32_t pageIdx = bitindex >> 3;
	*(img->data + pageIdx) &= ~(1 << (bitindex & 0x7));
}

