
#include "types.h"
#include "stringFunctions.h"
#include <string.h>
#include <stdlib.h>

void UInt8ToChar(uint8_t nr, char * out)
{
	uint8_t pos=100;
	uint8_t cntr=0,charpos=0;
	uint8_t firstDigit = 0;
	if (nr==0)
	{
		out[charpos++]=0x30;
	}
	else
	{
		while (pos > 0)
		{
			cntr=0;
			while (nr >= pos)
			{
				nr -= pos;
				cntr++;
			}
			if (cntr > 0 || firstDigit > 0)
			{
				out[charpos++] = cntr + 0x30;
				firstDigit = 1;
			}
			pos /= 10;
		}
	}
	out[charpos]=0;
}

void UInt16ToChar(uint16_t nr, char * out)
{
	uint16_t pos=10000;
	uint16_t cntr=0,charpos=0;
	uint16_t firstDigit = 0;
	if (nr==0)
	{
		out[charpos++]=0x30;
	}
	else
	{
		while (pos > 0)
		{
			cntr=0;
			while (nr >= pos)
			{
				nr -= pos;
				cntr++;
			}
			if (cntr > 0 || firstDigit > 0)
			{
				out[charpos++] = cntr + 0x30;
				firstDigit = 1;
			}
			pos /= 10;
		}
	}
	out[charpos]=0;
}

void UInt32ToChar(uint32_t nr, char * out)
{
	uint32_t pos=1000000000;
	uint32_t cntr=0,charpos=0;
	uint32_t firstDigit = 0;
	if (nr==0)
	{
		out[charpos++]=0x30;
	}
	else
	{
		while (pos > 0)
		{
			cntr=0;
			while (nr >= pos)
			{
				nr -= pos;
				cntr++;
			}
			if (cntr > 0 || firstDigit > 0)
			{
				out[charpos++] = cntr + 0x30;
				firstDigit = 1;
			}
			pos /= 10;
		}
	}
	out[charpos]=0;
}


uint8_t startsWith(const char* ptrn,const char* target)
{
	uint8_t cnt=0,isEqual=1;
	while (ptrn[cnt]>0 && target[cnt]>0 && isEqual > 0)
	{
		if (ptrn[cnt] != target[cnt])
		{
			isEqual=0;
		}
		cnt++;
	}
	return isEqual;
}

uint8_t toInt(char * chr)
{
	uint8_t res=0;
	uint8_t cnt=0;
	while (chr[cnt]!=0)
	{
		res *= 10;
		res += (chr[cnt] - 0x30);
		cnt++;
	}
	return res;
}

void getBracketContent(const char* input,char * out)
{
	uint8_t cnt=0,bcnt=0;
	uint8_t idxStart=255,idxEnd=0;
	char cur = *(input+cnt);
	while(cur != 0)
	{
		if (idxStart==255)
		{
			if (cur == '(')
			{
				idxStart = cnt;
			}
		}
		else if (cur != ')')
		{
			out[bcnt]=cur;
			bcnt++;
		}
		if (idxEnd==0)
		{
			if (cur == ')')
			{
				idxEnd = cnt;
			}
		}

		cnt++;
		cur = *(input+cnt);
	}
	out[bcnt]=0;
	if (bcnt==0 || idxEnd==0)
	{
		out = 0;
	}
}

void stripWhitespaces(char * input)
{

	uint8_t c1=0, c2=0;
	while(*(input+c1+c2) !=0)
	{
		if (*(input+c1)==0x20 || *(input+c1)==0x9)
		{
			c2++;
		}
		else
		{
			c1++;
		}
		*(input+c1)=*(input+c1+c2);
	}
	if(c2 > 0)
	{
		c1++;
		*(input+c1) = 0;
	}
}

uint8_t expandRange(char * stringinput,uint8_t ** result)
{
	char nr[4];
	uint8_t lowerBound=0;
	uint8_t upperBound=0;
	uint8_t c=0,c1=0;
	int len=0;
	while(*(stringinput+c) != '-' && *(stringinput+c) != 0)
	{
		nr[c1++]=stringinput[c++];
	}
	nr[c1]=0;

	lowerBound = toInt(nr);
	if(stringinput[c]!=0)
	{
		c++;
		c1=0;
		while(stringinput[c] != 0)
		{
			nr[c1++]=stringinput[c++];
		}
		nr[c1]=0;
		upperBound=toInt(nr);
	} else
	{
		upperBound=lowerBound;
	}

	len = (int)(upperBound-lowerBound+1);
	*result = (uint8_t*)malloc(len);
	for(uint8_t c=0;c<len;c++)
	{
		*(*(result) + c) = lowerBound + c;
	}
	return len;
}

