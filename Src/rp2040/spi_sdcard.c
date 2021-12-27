
#include "spi_sdcard.h"


void initSpi()
{
    // get spi out of reset
    *RESETS |= (1 << RESETS_RESET_SPI0_LSB); 
	*RESETS &= ~(1 << RESETS_RESET_SPI0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_SPI0_LSB)) == 0);

    // wire up the spi
    *MISO_PIN_CNTR = 1;
    *MOSI_PIN_CNTR = 1;
    *SCK_PIN_CNTR = 1;
    *CS_PIN_CNTR = 1;

    // enable pullups on mosi
    *MOSI_PAD_CNTR &= ~(1 << PADS_BANK0_GPIO0_PDE_LSB);
    *MOSI_PAD_CNTR |= (1 << PADS_BANK0_GPIO0_PUE_LSB);     

    // configure control register 0: 8-bit data, 200 as SCR resuting in a initial clock rate of 300 kHz at 120 MHz
    *SSPCR0 = (0x7 << SPI_SSPCR0_DSS_LSB) | (200 << SPI_SSPCR0_SCR_LSB);
    // configure clock divider
    *SSPCPSR = 2;
    // configure control register 1: enable by setting synchronous operation
    *SSPCR1 = 1 << SPI_SSPCR1_SSE_LSB;
}

void sendDummyBytes(uint16_t cnt)
{
    uint8_t dummy;
    for(uint16_t c=0;c<cnt;c++)
    {
        while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) ); 
        dummy = *SSPDR & 0xFF;
        *SSPDR = 0xFF;
    }
}

void sendCommand(uint8_t* cmd,uint8_t* resp,uint16_t len)
{
    // drive DO / MOSI high
    //*MOSI_PIN_CNTR =  5; // function 5 (SIO)
	//*GPIO_OE |= (1 << MOSI);
	//*(GPIO_OUT + 1) = (1 << MOSI);
    uint8_t c = 0;
    uint8_t retval;
    uint16_t retcnt=0;
    while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) ); // drain tx fifo
    for(c=0;c<6;c++) // send all commands at once
    {
        *SSPDR = cmd[c];
    }
    c=0;
    while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) ) // wait until command is sent, read back dummy bytes
    {
        if((*SSPSR & (1 << SPI_SSPSR_RNE_LSB))==(1 << SPI_SSPSR_RNE_LSB))
        {
            retval = *SSPDR & 0xFF;
            c++;
        }
    }
    while((*SSPSR & (1 << SPI_SSPSR_RNE_LSB))==(1 << SPI_SSPSR_RNE_LSB)) // read back remaining dummy bytes
    {
        retval = *SSPDR & 0xFF;
        c++;
    }
    // wait for response
    while(retcnt < len)
    {
        *SSPDR = 0xFF;
        while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) );
        //if ((*SSPSR & (1 << SPI_SSPSR_RNE_LSB))==(1 << SPI_SSPSR_RNE_LSB))
        //{
        retval = *SSPDR;
        if ((retcnt==0 && (retval & 0x80)==0) || retcnt > 0)
        {
            *(resp + retcnt++) = retval;
        }
        //}
    }
}

uint8_t initSdCard()
{
    uint8_t cmd[6];
    uint8_t resp[5];
    // send 10 dummy bytes
    sendDummyBytes(10);

    // send command 0
    cmd[0] = 0x40;
    cmd[1] = 0x0;
    cmd[2] = 0x0;
    cmd[3] = 0x0;
    cmd[4] = 0x0;
    cmd[5] = 0x95;
    sendCommand(cmd,resp,1);
    if (resp[0] != (1 << R1_IDLE))
    {
        return 1; // the card is probably unresponsive
    }

    // send command 8
    cmd[0] = 0x48;
    cmd[1] = 0x0;
    cmd[2] = 0x0;
    cmd[3] = 0x1;
    cmd[4] = 0xAA;
    cmd[5] = 0x87;
    sendCommand(cmd,resp,5);
    if (resp[0] == SD_CARD_VERSION_2)
    {
        for (uint8_t c2=1;c2<5;c2++)
        {
            if(cmd[c2] != resp[c2])
            {
                return 2;
            }
        }
        // send CMD55
        cmd[0] = 0x40 + 55;
        cmd[1] = 0x0;
        cmd[2] = 0x0;
        cmd[3] = 0x0;
        cmd[4] = 0x0;
        cmd[5] = 0xFF;
        sendCommand(cmd,resp,1);
        if((resp[0] & (1 << R1_ILLEGAL_COMMAND)) == (1 << R1_ILLEGAL_COMMAND))
        {
            // send command CMD1
            cmd[0] = 0x40 + 1;
            cmd[1] = 0x40;
            cmd[2] = 0x0;
            cmd[3] = 0x0;
            cmd[4] = 0x0;
            cmd[5] = 0xFF;
            sendCommand(cmd,resp,1);
        }
        else
        {
            // send command CMD41
            cmd[0] = 0x40 + 41;
            cmd[1] = 0x40;
            cmd[2] = 0x0;
            cmd[3] = 0x0;
            cmd[4] = 0x0;
            cmd[5] = 0xFF;
            sendCommand(cmd,resp,1);
        }
    }
    else{ // for sd card version 1 try CMD1 only
        // send command CMD1
        cmd[0] = 0x40 + 1;
        cmd[1] = 0x40;
        cmd[2] = 0x0;
        cmd[3] = 0x0;
        cmd[4] = 0x0;
        cmd[5] = 0xFF;
        sendCommand(cmd,resp,1);
    }

    if ((resp[0] & (1 << R1_ILLEGAL_COMMAND)) == (1 << R1_ILLEGAL_COMMAND))
    {
        return 3;
    }

    // set block size to 512 bytes
    // send CMD16
    cmd[0] = 0x40 + 16;
    cmd[1] = 0x0;
    cmd[2] = 0x0;
    cmd[3] = 0x2;
    cmd[4] = 0x0;
    cmd[5] = 0xFF;
    sendCommand(cmd,resp,1);
    if ((resp[0] & (1 << R1_ILLEGAL_COMMAND)) == (1 << R1_ILLEGAL_COMMAND))
    {
        return 3;
    }

    return 0;
}

