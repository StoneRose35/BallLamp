
#include "types.h"
#include "stringFunctions.h"

void toChar(uint8_t nr, char * out)
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
