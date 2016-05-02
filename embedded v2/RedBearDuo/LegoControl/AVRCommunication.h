#ifndef LEGOCONTROL_AVRCOMMUNICATION_H_
#define LEGOCONTROL_AVRCOMMUNICATION_H_

#include <string>
#include <vector>

#include "Arduino.h"

struct AVRCommandData {
  AVRCommandData(String pName, String pValue) {
    name = pName;
    value = pValue;
  }
  String name;
  String value;
};

class AVRCommandFactory {
public:
  static AVRCommandData createPingCommand();
  static AVRCommandData createMotorSpeedCommand(uint8_t speedPercent);
  static AVRCommandData createMotorDirectionCommand(bool isForward);
  static AVRCommandData createServoAngleCommand(float angle);
  static AVRCommandData createStatusColorCommand(uint32_t color);
};

class AVRProtocol {
public:
  static void init();
  static void stopAll();
  static void send(AVRCommandData command);
  static void send(std::vector<AVRCommandData> commands);
  static void ping();
};

#endif
