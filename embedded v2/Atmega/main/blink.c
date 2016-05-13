#include "blink.h"

void Blink_init(Blink* blink, Pin channelLeft, Pin channelRight, float tickPeriod, uint16_t blinkPeriodMs) {
    blink->pinChannelLeft = channelLeft;
    blink->pinChannelRight = channelRight;
    blink->tickPeriod = tickPeriod;
    blink->blinkPeriodMs = blinkPeriodMs;
    blink->currentPeriod = 0;
    
    Pin_setMode(&blink->pinChannelLeft, PinMode_Output);
    Pin_setMode(&blink->pinChannelRight, PinMode_Output);
    
    Pin_setValue(&blink->pinChannelLeft, PinValue_Low);
    Pin_setValue(&blink->pinChannelRight, PinValue_Low);
    
    blink->blinkMode = BlinkMode_Off;
}

void Blink_adaptValues(Blink* blink) {
    PinValue valueLeft = PinValue_Low;
    PinValue valueRight = PinValue_Low;
    
    switch(blink->blinkMode) {
        case BlinkMode_Off:
            break;
        case BlinkMode_Right:
            valueRight = blink->on ? PinValue_High : PinValue_Low;
            break;
        case BlinkMode_Left:
            valueLeft = blink->on ? PinValue_High : PinValue_Low;
            break;
        case BlinkMode_Both:
            valueRight = blink->on ? PinValue_High : PinValue_Low;
            valueLeft = blink->on ? PinValue_High : PinValue_Low;
            break;
        default:
            break;
    }

    Pin_setValue(&blink->pinChannelLeft, valueLeft);
    Pin_setValue(&blink->pinChannelRight, valueRight);
}

void Blink_tick(Blink* blink) {
    if(blink->blinkMode == BlinkMode_Off) {
        return;
    }
    blink->currentPeriod += blink->tickPeriod;
    if(blink->currentPeriod > blink->blinkPeriodMs) {
        blink->currentPeriod -= blink->blinkPeriodMs;
        blink->on = !blink->on;
        Blink_adaptValues(blink);
    }
}

void Blink_setMode(Blink* blink, BlinkMode mode) {
    blink->blinkMode = mode;
    //reset interval values on mode change
    blink->on = 0;
    blink->currentPeriod = 0;
    Blink_adaptValues(blink);
}