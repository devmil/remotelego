#ifndef BLINK_H_
#define BLINK_H_

#include "Pin.h"

typedef enum {
    BlinkMode_Off   = 0,
    BlinkMode_Right = 1,
    BlinkMode_Left  = 2,
    BlinkMode_Both  = 3,
} BlinkMode;

typedef struct {
    Pin pinChannelLeft;
    Pin pinChannelRight;
    float tickPeriod;
    float currentPeriod;
    uint16_t blinkPeriodMs;
    BlinkMode blinkMode;
    uint8_t on;
} Blink;

void Blink_init(Blink* blink, Pin channelLeft, Pin channelRight, float tickPeriod, uint16_t blinkPeriodMs);

void Blink_tick(Blink* blink);

void Blink_setMode(Blink* blink, BlinkMode mode);

#endif