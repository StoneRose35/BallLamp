
#include "rotaryEncoder.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/m0plus.h"
#include "hardware/regs/sio.h"
#include "systick.h"

static uint32_t oldtickenc,oldtickswitch;
static volatile uint32_t encoderVal = 0x7FFFFFFF;
static volatile uint8_t switchVal;
static volatile uint8_t switchPins[8];
static volatile uint8_t switchVals[8];
static volatile uint32_t oldTickSwitches[8];
static volatile uint8_t lastTrigger;
void isr_io_irq_bank0_irq13()
{
    uint32_t* switchIntAddress;
    if ((*ENCODER_1_INTR & (1 << ENCODER_1_EDGE_HIGH)) == (1 << ENCODER_1_EDGE_HIGH))
    {
        *ENCODER_1_INTR |= (1 << ENCODER_1_EDGE_HIGH);
        if(lastTrigger == 1)
        {
            ((*ENCODER_2_IN & (1 << ENCODER_2)) == (1 << ENCODER_2)) ? encoderVal-- : encoderVal++;
            //oldtickenc = getTickValue();
        }
        lastTrigger = 0;
    }
    else if ((*ENCODER_1_INTR & (1 << ENCODER_1_EDGE_LOW)) == (1 << ENCODER_1_EDGE_LOW))
    {
        *ENCODER_1_INTR |= (1 << ENCODER_1_EDGE_LOW);
        if(lastTrigger==1)
        {
            ((*ENCODER_2_IN & (1 << ENCODER_2)) == (1 << ENCODER_2)) ? encoderVal++ : encoderVal--;
            //oldtickenc = getTickValue();
        }
        lastTrigger = 0;
    }
    else if ((*ENCODER_2_INTR & (1 << ENCODER_2_EDGE_HIGH)) == (1 << ENCODER_2_EDGE_HIGH))
    {
        *ENCODER_2_INTR |= (1 << ENCODER_2_EDGE_HIGH);
        lastTrigger = 1;
    }
    else if ((*ENCODER_2_INTR & (1 << ENCODER_2_EDGE_LOW)) == (1 << ENCODER_2_EDGE_LOW))
    {
        *ENCODER_2_INTR |= (1 << ENCODER_2_EDGE_LOW);
        lastTrigger = 1;
    }


    for (uint8_t c=0;c<8;c++)
    {
        switchIntAddress = (uint32_t*)(IO_BANK0_BASE + IO_BANK0_INTR0_OFFSET + (((4*switchPins[c]) & 0xFFE0) >> 3)); 
        if ((*switchIntAddress & (1 << (((4*switchPins[c]) & 0x1F)+3))) == (1 << (((4*switchPins[c]) & 0x1F)+3)))
        {
            *switchIntAddress |= (1 << (((4*switchPins[c]) & 0x1F)+3));
            if (oldTickSwitches[c] + ROTARY_ENCODER_DEBOUNCE < getTickValue())
            {
                switchVals[c]=0;
                oldTickSwitches[c]=getTickValue();
            }
        }
        else if ((*switchIntAddress & (1 << (((4*switchPins[c]) & 0x1F)+2))) == (1 << (((4*switchPins[c]) & 0x1F)+2)))
        {
            *switchIntAddress |= (1 << (((4*switchPins[c]) & 0x1F)+2));
            if (oldTickSwitches[c] + ROTARY_ENCODER_DEBOUNCE < getTickValue())
            {
                switchVals[c]=1;
                oldTickSwitches[c]=getTickValue();
            }
        }
    }/*
    else if ((*SWITCH_INTR & (1 << SWITCH_EDGE_HIGH)) == (1 << SWITCH_EDGE_HIGH))
    {
        *SWITCH_INTR |= (1 << SWITCH_EDGE_HIGH);
        if (oldtickswitch + ROTARY_ENCODER_DEBOUNCE  < getTickValue())
        {
            switchVal=0;
            oldtickswitch=getTickValue();
        }
    }
    else if ((*SWITCH_INTR & (1 << SWITCH_EDGE_LOW)) == (1 << SWITCH_EDGE_LOW))
    {
        *SWITCH_INTR |= (1 << SWITCH_EDGE_LOW);
        if (oldtickswitch + ROTARY_ENCODER_DEBOUNCE  < getTickValue())
        {
            switchVal=1;
            oldtickswitch=getTickValue();
        }
    }
*/

}

void initRotaryEncoder(const uint8_t* pins,const uint8_t nswitches)
{
    uint32_t* switchInteAddress;
    // define pullups for encoder input and switch
    *ENCODER_1_PAD_CNTR &= ~(1 << PADS_BANK0_GPIO0_PDE_LSB);
    *ENCODER_1_PAD_CNTR |= (1 << PADS_BANK0_GPIO0_PUE_LSB);
    *ENCODER_2_PAD_CNTR &= ~(1 << PADS_BANK0_GPIO0_PDE_LSB);
    *ENCODER_2_PAD_CNTR |= (1 << PADS_BANK0_GPIO0_PUE_LSB);

    *SWITCH_PAD_CNTR &= ~(1 << PADS_BANK0_GPIO0_PDE_LSB);
    *SWITCH_PAD_CNTR |= (1 << PADS_BANK0_GPIO0_PUE_LSB);

    // set io bank control to sio
    *ENCODER_1_PIN_CNTR = 5;
    *ENCODER_2_PIN_CNTR = 5;
    *SWITCH_PIN_CNTR = 5;

    // enable level change interrupt
    *ENCODER_1_INTE |= (1 << ENCODER_1_EDGE_LOW) | (1 << ENCODER_1_EDGE_HIGH);
    *ENCODER_2_INTE |= (1 << ENCODER_2_EDGE_LOW) | (1 << ENCODER_2_EDGE_HIGH);
    for (uint8_t c=0;c<nswitches;c++)
    {
        switchInteAddress = (uint32_t*)(IO_BANK0_BASE + IO_BANK0_PROC0_INTE0_OFFSET + (((4*pins[c]) & 0xFFE0) >> 3));
        *switchInteAddress |= (1 << (((4*pins[c]) & 0x1F)+2)) | (1 << (((4*pins[c]) & 0x1F)+3)); // (1 << SWITCH_EDGE_HIGH) | (1 << SWITCH_EDGE_LOW);
        switchPins[c]=pins[c];
    }
    //*SWITCH_INTE |= (1 << SWITCH_EDGE_HIGH) | (1 << SWITCH_EDGE_LOW);
    *NVIC_ISER = (1 << 13);

    //read old tick values
    oldtickenc=getTickValue();
    oldtickswitch=getTickValue();
    lastTrigger = 0;
}

uint32_t getEncoderValue()
{
    return encoderVal;
}

uint8_t getSwitchValue(uint8_t nr)
{
    return switchVals[nr];
}