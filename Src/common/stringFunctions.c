
#include "types.h"
#include "stringFunctions.h"
#include <string.h>
#include <stdlib.h>


void toPercentChar(float percentVal,char * out)
{
	uint32_t ival = (uint32_t)(percentVal*100000.0);
	UInt32ToChar(ival,out);
	fillWithLeadingZeros(4,out);
	//re-insert comma
	uint8_t str_len = 0;
	char swap, swap2;
	while (*(out+str_len) != 0)
	{
		str_len++;
	}
	swap = out[str_len-3];
	out[str_len-3] = '.';
	for(uint8_t c=str_len-2;c<str_len+1;c++)
	{
		swap2 = out[c];
		out[c] = swap;
		swap = swap2;
	}
	out[str_len+1] = 0;
}

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
	uint16_t npos=0;
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


void fillWithLeadingZeros(uint8_t minlength,char * nr)
{
	uint8_t nrlen=0;
	while(nr[nrlen] != 0)
	{
		nrlen++;
	}
	if (nrlen < minlength)
	{
		for(uint8_t c2=0;c2<minlength-nrlen;c2++)
		{
			for(uint8_t c3=nrlen+c2;c3>c2;c3--)
			{
				nr[c3]=nr[c3-1];
			}
			nr[c2]=0x30;
		}
		nr[minlength] = 0;
	}
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

uint8_t toUInt8(char * chr)
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

uint32_t toUInt32(char * chr)
{
	uint32_t res=0;
	uint32_t cnt=0;
	while (chr[cnt]!=0)
	{
		res *= 10;
		res += (chr[cnt] - 0x30);
		cnt++;
	}
	return res;
}


int16_t toInt16(char * chr)
{
	int16_t res=0;
	int16_t cnt=0;
	uint8_t hasMinus = 0;
	if(chr[cnt] == '-')
	{
		hasMinus = 1;
		cnt++;
	}
	while (chr[cnt]!=0)
	{
		res *= 10;
		res += (chr[cnt] - 0x30);
		cnt++;
	}
	if (hasMinus == 1)
	{
		res = -res;
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

	lowerBound = toUInt8(nr);
	if(stringinput[c]!=0)
	{
		c++;
		c1=0;
		while(stringinput[c] != 0)
		{
			nr[c1++]=stringinput[c++];
		}
		nr[c1]=0;
		upperBound=toUInt8(nr);
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

