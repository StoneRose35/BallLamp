
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

void isr_io_irq_bank0_irq13()
{
    if ((*ENCODER_1_INTR & (1 << ENCODER_1_EDGE_HIGH)) == (1 << ENCODER_1_EDGE_HIGH))
    {
        *ENCODER_1_INTR |= (1 << ENCODER_1_EDGE_HIGH);
        if(oldtickenc + ROTARY_ENCODER_DEBOUNCE < getTickValue())
        {
            ((*ENCODER_2_IN & (1 << ENCODER_2)) == (1 << ENCODER_2)) ? encoderVal-- : encoderVal++;
            oldtickenc = getTickValue();
        }
    }
    else if ((*ENCODER_1_INTR & (1 << ENCODER_1_EDGE_LOW)) == (1 << ENCODER_1_EDGE_LOW))
    {
        *ENCODER_1_INTR |= (1 << ENCODER_1_EDGE_LOW);
        if(oldtickenc + ROTARY_ENCODER_DEBOUNCE < getTickValue())
        {
            ((*ENCODER_2_IN & (1 << ENCODER_2)) == (1 << ENCODER_2)) ? encoderVal++ : encoderVal--;
            oldtickenc = getTickValue();
        }
    }
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


}

void initRotaryEncoder()
{
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
    *ENCODER_1_INTE |=  (1 << ENCODER_1_EDGE_LOW) | (1 << ENCODER_1_EDGE_HIGH);
    *SWITCH_INTE |= (1 << SWITCH_EDGE_HIGH) | (1 << SWITCH_EDGE_LOW);
    *NVIC_ISER = (1 << 13);

    //read old tick values
    oldtickenc=getTickValue();
    oldtickswitch=getTickValue();
}

uint32_t getEncoderValue()
{
    return encoderVal;
}

uint8_t getSwitchValue()
{
    return switchVal;
}