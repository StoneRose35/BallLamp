#include "audio/waveShaper.h"
const WaveShaperDataType waveShaperUnity = {
    .transferFunctionPoints = {
        0x8001, 0x8206, 0x840a, 0x860e, 0x8812, 0x8a16, 0x8c1a, 0x8e1e, 0x9022, 0x9226, 0x942a, 0x962e, 0x9832, 0x9a36, 0x9c3a, 0x9e3e, 0xa042, 0xa246, 0xa44a, 0xa64e, 0xa852, 0xaa56, 0xac5a, 0xae5e, 0xb062, 0xb266, 0xb46a, 0xb66e, 0xb872, 0xba76, 0xbc7a, 0xbe7e, 0xc082, 0xc286, 0xc48a, 0xc68e, 0xc892, 0xca96, 0xcc9a, 0xce9e, 0xd0a2, 0xd2a6, 0xd4aa, 0xd6ae, 0xd8b2, 0xdab6, 0xdcba, 0xdebe, 0xe0c2, 0xe2c6, 0xe4ca, 0xe6ce, 0xe8d2, 0xead6, 0xecda, 0xeede, 0xf0e2, 0xf2e6, 0xf4ea, 0xf6ee, 0xf8f2, 0xfaf6, 0xfcfa, 0xfefe, 0x102, 0x306, 0x50a, 0x70e, 0x912, 0xb16, 0xd1a, 0xf1e, 0x1122, 0x1326, 0x152a, 0x172e, 0x1932, 0x1b36, 0x1d3a, 0x1f3e, 0x2142, 0x2346, 0x254a, 0x274e, 0x2952, 0x2b56, 0x2d5a, 0x2f5e, 0x3162, 0x3366, 0x356a, 0x376e, 0x3972, 0x3b76, 0x3d7a, 0x3f7e, 0x4182, 0x4386, 0x458a, 0x478e, 0x4992, 0x4b96, 0x4d9a, 0x4f9e, 0x51a2, 0x53a6, 0x55aa, 0x57ae, 0x59b2, 0x5bb6, 0x5dba, 0x5fbe, 0x61c2, 0x63c6, 0x65ca, 0x67ce, 0x69d2, 0x6bd6, 0x6dda, 0x6fde, 0x71e2, 0x73e6, 0x75ea, 0x77ee, 0x79f2, 0x7bf6, 0x7dfa, 0x7fff, 
    }
};

const WaveShaperDataType waveShaperDefaultOverdrive = {
    .transferFunctionPoints = {
        0x8001, 0x8028, 0x804f, 0x8076, 0x809c, 0x80c3, 0x80ea, 0x8110, 0x8137, 0x815e, 0x8185, 0x81ab, 0x81d2, 0x81f9, 0x821f, 0x8246, 0x826d, 0x8293, 0x82ba, 0x82e1, 0x8308, 0x832e, 0x8355, 0x837c, 0x83a2, 0x83c9, 0x8421, 0x8562, 0x8772, 0x8a22, 0x8d41, 0x90a0, 0x9410, 0x977d, 0x9aea, 0x9e58, 0xa1c5, 0xa532, 0xa89f, 0xac0c, 0xaf7a, 0xb2e7, 0xb654, 0xb9c1, 0xbd2f, 0xc09c, 0xc409, 0xc776, 0xcae3, 0xce51, 0xd1be, 0xd52b, 0xd898, 0xdc06, 0xdf73, 0xe2e0, 0xe64d, 0xe9bb, 0xed28, 0xf095, 0xf402, 0xf76f, 0xfadd, 0xfe4a, 0x1b6, 0x523, 0x891, 0xbfe, 0xf6b, 0x12d8, 0x1645, 0x19b3, 0x1d20, 0x208d, 0x23fa, 0x2768, 0x2ad5, 0x2e42, 0x31af, 0x351d, 0x388a, 0x3bf7, 0x3f64, 0x42d1, 0x463f, 0x49ac, 0x4d19, 0x5086, 0x53f4, 0x5761, 0x5ace, 0x5e3b, 0x61a8, 0x6516, 0x6883, 0x6bf0, 0x6f60, 0x72bf, 0x75de, 0x788e, 0x7a9e, 0x7bdf, 0x7c37, 0x7c5e, 0x7c84, 0x7cab, 0x7cd2, 0x7cf8, 0x7d1f, 0x7d46, 0x7d6d, 0x7d93, 0x7dba, 0x7de1, 0x7e07, 0x7e2e, 0x7e55, 0x7e7b, 0x7ea2, 0x7ec9, 0x7ef0, 0x7f16, 0x7f3d, 0x7f64, 0x7f8a, 0x7fb1, 0x7fd8, 0x7fff
    }
};

const WaveShaperDataType waveShaperSoftOverdrive = {
    .transferFunctionPoints = {
        0x8001, 0x8013, 0x8024, 0x8036, 0x804b, 0x8063, 0x8082, 0x80a8, 0x80d8, 0x8112, 0x8159, 0x81ae, 0x8212, 0x8288, 0x8311, 0x83af, 0x8463, 0x852f, 0x8615, 0x8716, 0x8834, 0x8971, 0x8acd, 0x8c4c, 0x8dee, 0x8fb6, 0x91a4, 0x93bb, 0x95fb, 0x9868, 0x9b02, 0x9dcb, 0xa0c3, 0xa3c9, 0xa6cf, 0xa9d5, 0xacdb, 0xafe1, 0xb2e7, 0xb5ed, 0xb8f3, 0xbbf9, 0xbeff, 0xc205, 0xc50b, 0xc811, 0xcb17, 0xce1d, 0xd123, 0xd429, 0xd72f, 0xda35, 0xdd3b, 0xe041, 0xe347, 0xe64d, 0xe953, 0xec59, 0xef5f, 0xf265, 0xf56b, 0xf871, 0xfb77, 0xfe7d, 0x183, 0x489, 0x78f, 0xa95, 0xd9b, 0x10a1, 0x13a7, 0x16ad, 0x19b3, 0x1cb9, 0x1fbf, 0x22c5, 0x25cb, 0x28d1, 0x2bd7, 0x2edd, 0x31e3, 0x34e9, 0x37ef, 0x3af5, 0x3dfb, 0x4101, 0x4407, 0x470d, 0x4a13, 0x4d19, 0x501f, 0x5325, 0x562b, 0x5931, 0x5c37, 0x5f3d, 0x6235, 0x64fe, 0x6798, 0x6a05, 0x6c45, 0x6e5c, 0x704a, 0x7212, 0x73b4, 0x7533, 0x768f, 0x77cc, 0x78ea, 0x79eb, 0x7ad1, 0x7b9d, 0x7c51, 0x7cef, 0x7d78, 0x7dee, 0x7e52, 0x7ea7, 0x7eee, 0x7f28, 0x7f58, 0x7f7e, 0x7f9d, 0x7fb5, 0x7fca, 0x7fdc, 0x7fed, 0x7fff
    }
};

const WaveShaperDataType waveShaperDistortion = {
    .transferFunctionPoints = {
        0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8000, 0x8041, 0x8198, 0x84d3, 0x88bc, 0x8ca5, 0x908e, 0x9477, 0x9860, 0x9c49, 0xa032, 0xa41b, 0xa804, 0xabed, 0xafd7, 0xb3c0, 0xb7a9, 0xbb92, 0xbf7b, 0xc364, 0xc74d, 0xcb36, 0xcf1f, 0xd308, 0xd6f1, 0xdada, 0xdec3, 0xe2ac, 0xe696, 0xea7f, 0xee68, 0xf251, 0xf63a, 0xfa23, 0xfe0c, 0x1f4, 0x5dd, 0x9c6, 0xdaf, 0x1198, 0x1581, 0x196a, 0x1d54, 0x213d, 0x2526, 0x290f, 0x2cf8, 0x30e1, 0x34ca, 0x38b3, 0x3c9c, 0x4085, 0x446e, 0x4857, 0x4c40, 0x5029, 0x5413, 0x57fc, 0x5be5, 0x5fce, 0x63b7, 0x67a0, 0x6b89, 0x6f72, 0x735b, 0x7744, 0x7b2d, 0x7e68, 0x7fbf, 0x8000, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff

    }
};



void initWaveShaper(WaveShaperDataType * data,const WaveShaperDataType* copyFrom)
{
    for (uint8_t c=0;c<128;c++)
    {
        data->transferFunctionPoints[c]=copyFrom->transferFunctionPoints[c];
    }
}

int16_t waveShaperProcessSample(int16_t sampleIn,WaveShaperDataType* data)
{
    uint32_t indx;
    uint16_t sampleUint;
    uint32_t rem;
    int16_t out;
    sampleUint = 0x7FFF + sampleIn;
    indx = ((((uint16_t)sampleUint) & 0xFE00) >> 9);
    rem = sampleUint - (uint16_t)((sampleUint) & 0xFE00);
    if (indx < 127)
    {
        out = data->transferFunctionPoints[indx] + (((int16_t)rem*(data->transferFunctionPoints[indx+1]-data->transferFunctionPoints[indx])) >> 9);
    }
    else
    {
        out = data->transferFunctionPoints[indx];
    }
    return out;
}