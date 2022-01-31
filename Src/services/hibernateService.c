
#include "services/hibernateService.h"
#include "systick.h"
#include "uiStack.h"
#include "spi_sdcard_display.h"
#include "rotaryEncoder.h"

static volatile HibernateServiceDataType hibernateServiceData;

volatile HibernateServiceDataType * getHibernateServiceData()
{
    return &hibernateServiceData;
}

uint8_t initHibernateService()
{
    hibernateServiceData.lastActionTick = getTickValue();
    hibernateServiceData.lastEncoderVal = getEncoderValue();
    hibernateServiceData.lastSwitchVal = getSwitchValue();
    hibernateServiceData.lastBrightnessValue = getBrightness();
    return 0;
}
uint8_t  hibernateServiceLoop()
{
    uint32_t cticks = getTickValue();
    if ((cticks - hibernateServiceData.lastActionTick > 6000) && hibernateServiceData.hibernateState == HIBERNATE_AWAKE)
    {
        // enter hibernation
        displayOff();
        hibernateServiceData.lastBrightnessValue = getBrightness();
        setBrightness(0);
        hibernateServiceData.lastActionTick = cticks;
        hibernateServiceData.hibernateState = HIBERNATE_SLEEPING;
        return HIBERNATE_SLEEP;
    }
    else if (hibernateServiceData.hibernateState == 1)
    {
        if (getSwitchValue() != hibernateServiceData.lastSwitchVal || getEncoderValue() != hibernateServiceData.lastEncoderVal)
        {
            displayOn();
            setBrightness(hibernateServiceData.lastBrightnessValue);
            hibernateServiceData.hibernateState = HIBERNATE_AWAKE;
            hibernateServiceData.lastSwitchVal = getSwitchValue();
            hibernateServiceData.lastEncoderVal = getEncoderValue();
            return HIBERNATE_WAKEUP;
        }
    }
    return 0;
}