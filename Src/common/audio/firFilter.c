#include "audio/firFilter.h"
#include "multicore.h"

void initfirFilter(FirFilterType*data)
{
    data->filterLength=64;
    data->delayPointer=0;
    for(uint8_t c=0;c<data->filterLength;c++)
    {
        //data->coefficients[c]=0;
        data->delayBuffer[c]=0;
    }
}

void addSample(int16_t sampleIn,FirFilterType*data)
{
    data->delayBuffer[data->delayPointer--]=sampleIn;
    data->delayPointer &= (data->filterLength-1);
}

int16_t firFilterProcessSample(int16_t sampleIn,FirFilterType*data)
{
    int16_t firstHalf,secondHalf;
    addSample(sampleIn,data);
    while ((*SIO_FIFO_ST & ( 1 << SIO_FIFO_ST_RDY_LSB)) == 0);
    // send pointer to data to core1 to indicate that a fir calculation has to be made
    *SIO_FIFO_WR = (uint32_t)data;
    firstHalf = processFirstHalf(data);
    // wait for core1 to be finished, core 1 sends the result of the second half back
    while ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB)) != (1 << SIO_FIFO_ST_VLD_LSB));
    secondHalf = (int16_t)*SIO_FIFO_RD;
    return firstHalf+secondHalf;
}

int16_t processFirstHalf(FirFilterType*data)
{
    int16_t res=0;
    uint8_t runningPtr=(data->delayPointer+1) & (data->filterLength-1);
    for(uint8_t c=0;c<(data->filterLength >> 1);c++)
    {
        res += (data->delayBuffer[runningPtr]*data->coefficients[runningPtr]) >> 15;
        runningPtr += 1;
        runningPtr &= (data->filterLength-1);
    }
    return res;
}

int16_t processSecondHalf(FirFilterType*data)
{
    int16_t res=0;
    uint8_t runningPtr=(data->delayPointer+1+(data->filterLength>>1)) & (data->filterLength-1);
    for(uint8_t c=0;c<(data->filterLength >> 1);c++)
    {
        res += (data->delayBuffer[runningPtr]*data->coefficients[runningPtr]) >> 15;
        runningPtr += 1;
        runningPtr &= (data->filterLength-1);
    }
    return res;
}