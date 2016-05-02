#ifndef LEGOCONTROL_AVRCOMMUNICATION_H_
#define LEGOCONTROL_AVRCOMMUNICATION_H_

#include "Arduino.h"

void AVR_configure();

void AVR_sendCommands(String commands, uint8_t numOfCommands);

void AVR_stopAll();

void AVR_ping();

#endif
