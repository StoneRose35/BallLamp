
/**
 * @file stringFunctions.c
 * @author Philipp	Fuerholz (fuerholz@gmx.ch)
 * @brief contains various string conversion functions
 * @version 0.1
 * @date 2021-12-23
 * 
 * 
 */
#include <stdint.h>
#include "stringFunctions.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief converts a number from 0 1 into a string showing percent from 0 to 100. The precision is fixed to three digits.
 * 
 * @param percentVal the value to convert
 * @param out the character array holding the converted string, must be initialized and of length 8 minimum
 */
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

/**
 * @brief converts a uint8_t to a string
 * 
 * @param nr the number to convert
 * @param out the character array,must be initialized and of length 4 minimum
 */
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

/**
 * @brief converts a uint16_t to a string
 * 
 * @param nr the number to convert
 * @param out the character array,must be initialized and of length 6 minimum
 */
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

/**
 * @brief converts a uint32_t to a string
 * 
 * @param nr the number to convert
 * @param out the character array,must be initialized and of length 11 minimum
 */
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

/**
 * @brief converts a 32bit unsigned int to a hex representation, fox example UInt32ToHex(123,nrbfr)
 * would put "0x7b" in nrbfr
 * @param val the unsigned int value to convert
 * @param nrbfr the buffer to hold the string representation, must be at least 11 chars in size
 */
void UInt32ToHex(uint32_t val,char* nrbfr)
{
	uint32_t cval = val;
	uint32_t nibbleval;
	uint8_t charcnt=0;
	if (val == 0)
	{
		nrbfr[0] = '0';
		nrbfr[1] = 'x';
		nrbfr[2] = '0';
		charcnt = 3;
	}
	else
	{
		nrbfr[0] = '0';
		nrbfr[1] = 'x';
		charcnt = 2;
		while (charcnt < 10)
		{
			// shift out left, discard leading zeros
			nibbleval = cval >> 28;
			if ((nibbleval < 10 && nibbleval > 0) || ( nibbleval < 10 && charcnt > 2))
			{
				nrbfr[charcnt++] = nibbleval + 0x30;
			}
			else if (nibbleval > 10)
			{
				nrbfr[charcnt++] = nibbleval + 0x61 - 10;
			}
			cval = cval <<  4;
		}
	}
	nrbfr[charcnt] = 0;
}



/**
 * @brief add zeros on the left side on a string representing an integer number, used by toPercentChar
 * 
 * @param minlength the minimum length the string should have
 * @param nr the array containing the string representation of the integer number, note the extra space must be allocated
 */
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

/**
 * @brief checks if one string start with another
 * 
 * @param ptrn the pattern to check
 * @param target the string for which should be checked if it start with ptrn
 * @return 1 if target start with ptrn, 0 otherwise
 */
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

/**
 * @brief converts the string to uppercase letters
 * 
 * @param str 
 */
void toUpper(char * str,char endchar)
{
	uint16_t c=0;
	uint8_t endcharFound=0;
	while((uint8_t)*(str+c) != 0 && endcharFound == 0)
	{
		if ((uint8_t)*(str+c) >=97 && (uint8_t)*(str+c) <=122)
		{
			*((uint8_t*)str+c)=(uint8_t)*(str+c) - 32;
		}
		c++;
		if(endchar > 0)
		{
			endcharFound = *(str+c) == endchar;
		}
	}
}

/**
 * @brief convert a string to an uint8_t, doesn't do any validity check on the string
 * 
 * @param chr the string convert
 * @return the string as a uint8_t
 */
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

/**
 * @brief convert a string to an uint32_t, doesn't do any validity check on the string
 * 
 * @param chr the string convert
 * @return the string as a uint32_t
 */
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

/**
 * @brief convert a string to an int16_t, doesn't do any validity check on the string
 * 
 * @param chr the string convert
 * @return the string as a int16_t
 */
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

/**
 * @brief returns the content within the brackts of a string. Doens't do any specific validity checks. For example for "RGB(123,32,34,0-17)"
 * the function would return "123,32,34,0-17"
 * 
 * @param input the string containing exactly one set of brackett: "()"
 * @param out the content within the brackets
 */
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

/**
 * @brief removes all whitespace characters (Tab, Space) from a string
 * 
 * @param input the string for which the whitespace should be removed, note that the operation happens in-place and that the character after the terminating 0 are not zeroed
 */
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

/**
 * @brief expands a range description from uint8_t values into a uint8_t array. The first number must be smaller than the seconds one. Also the string should no contains whitespaces.
 * 
 * @param stringinput the string which should be expanded
 * @param result pointer to the uint8_t array, the array itself is initialized dynamically within the function
 * @return the length of the array, 0 if no range has been input
 */
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

