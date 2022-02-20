#include "audio/sineplayer.h"
#include <stdint.h>

const uint32_t phaseincrements[128]={
976512,
1034579,
1096098,
1161276,
1230329,
1303488,
1380997,
1463116,
1550117,
1642292,
1739948,
1843410,
1953025,
2069158,
2192197,
2322552,
2460658,
2606976,
2761995,
2926232,
3100235,
3284584,
3479896,
3686821,
3906051,
4138317,
4384394,
4645104,
4921316,
5213953,
5523991,
5852464,
6200470,
6569169,
6959792,
7373643,
7812103,
8276635,
8768789,
9290208,
9842633,
10427906,
11047982,
11704929,
12400940,
13138339,
13919585,
14747287,
15624206,
16553270,
17537578,
18580417,
19685266,
20855813,
22095964,
23409859,
24801881,
26276678,
27839171,
29494574,
31248413,
33106540,
35075157,
37160835,
39370533,
41711627,
44191929,
46819718,
49603763,
52553357,
55678342,
58989149,
62496826,
66213081,
70150315,
74321670,
78741067,
83423254,
88383859,
93639437,
99207527,
105106714,
111356684,
117978298,
124993652,
132426162,
140300631,
148643341,
157482134,
166846509,
176767718,
187278874,
198415055,
210213429,
222713369,
235956596,
249987305,
264852324,
280601262,
297286682,
314964268,
333693018,
353535437,
374557748,
396830111,
420426858,
445426739,
471913192,
499974610,
529704648,
561202525,
594573364,
629928536,
667386036,
707070875,
749115497,
793660223,
840853716,
890853479,
943826384,
999949221,
1059409296,
1122405051,
1189146729,
1259857073,
1334772073,
1414141751,
1498230995
};

const int16_t sinevals[256]={
32767,
32757,
32727,
32678,
32609,
32520,
32412,
32284,
32137,
31970,
31785,
31580,
31356,
31113,
30851,
30571,
30272,
29955,
29621,
29268,
28897,
28510,
28105,
27683,
27244,
26789,
26318,
25831,
25329,
24811,
24278,
23731,
23169,
22594,
22004,
21402,
20787,
20159,
19519,
18867,
18204,
17530,
16845,
16150,
15446,
14732,
14009,
13278,
12539,
11792,
11038,
10278,
9511,
8739,
7961,
7179,
6392,
5601,
4807,
4011,
3211,
2410,
1607,
804,
0,
-804,
-1607,
-2410,
-3211,
-4011,
-4807,
-5601,
-6392,
-7179,
-7961,
-8739,
-9511,
-10278,
-11038,
-11792,
-12539,
-13278,
-14009,
-14732,
-15446,
-16150,
-16845,
-17530,
-18204,
-18867,
-19519,
-20159,
-20787,
-21402,
-22004,
-22594,
-23169,
-23731,
-24278,
-24811,
-25329,
-25831,
-26318,
-26789,
-27244,
-27683,
-28105,
-28510,
-28897,
-29268,
-29621,
-29955,
-30272,
-30571,
-30851,
-31113,
-31356,
-31580,
-31785,
-31970,
-32137,
-32284,
-32412,
-32520,
-32609,
-32678,
-32727,
-32757,
-32767,
-32757,
-32727,
-32678,
-32609,
-32520,
-32412,
-32284,
-32137,
-31970,
-31785,
-31580,
-31356,
-31113,
-30851,
-30571,
-30272,
-29955,
-29621,
-29268,
-28897,
-28510,
-28105,
-27683,
-27244,
-26789,
-26318,
-25831,
-25329,
-24811,
-24278,
-23731,
-23169,
-22594,
-22004,
-21402,
-20787,
-20159,
-19519,
-18867,
-18204,
-17530,
-16845,
-16150,
-15446,
-14732,
-14009,
-13278,
-12539,
-11792,
-11038,
-10278,
-9511,
-8739,
-7961,
-7179,
-6392,
-5601,
-4807,
-4011,
-3211,
-2410,
-1607,
-804,
0,
804,
1607,
2410,
3211,
4011,
4807,
5601,
6392,
7179,
7961,
8739,
9511,
10278,
11038,
11792,
12539,
13278,
14009,
14732,
15446,
16150,
16845,
17530,
18204,
18867,
19519,
20159,
20787,
21402,
22004,
22594,
23169,
23731,
24278,
24811,
25329,
25831,
26318,
26789,
27244,
27683,
28105,
28510,
28897,
29268,
29621,
29955,
30272,
30571,
30851,
31113,
31356,
31580,
31785,
31970,
32137,
32284,
32412,
32520,
32609,
32678,
32727,
32757
};

volatile uint32_t currentPhase=0;
volatile uint8_t currentNote=63;

uint16_t getNextSineValue()
{
    uint16_t v1,v2;
    uint32_t nval;
    v1=sinevals[((currentPhase >> 24) & 0xFF)];
    v2=sinevals[(((currentPhase + phaseincrements[currentNote])>>24) & 0xFF)];
    nval = v1 + ((((phaseincrements[currentNote] >> 8) & 0xFFFF)*(v2-v1)) >> 16);
    currentPhase += phaseincrements[currentNote];
    return nval;
}

void setNote(uint8_t note)
{
    currentNote = note;
}
uint8_t getNote()
{
    return currentNote;
}



