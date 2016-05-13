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

enum class Position {
  Left,
  Right,
  Both
};

class AVRCommandFactory {
public:
  static AVRCommandData createPingCommand();
  static AVRCommandData createMotorSpeedCommand(uint8_t speedPercent);
  static AVRCommandData createMotorDirectionCommand(bool isForward);
  static AVRCommandData createServoAngleCommand(float angle);
  static AVRCommandData createStatusColorCommand(uint32_t color);
  
  static AVRCommandData createFrontHeadlightCommand(bool on);
  static AVRCommandData createRearLightCommand(bool on);
  static AVRCommandData createFrontFoglightCommand(Position pos, bool on);
  static AVRCommandData createReversingLightCommand(bool on);
  static AVRCommandData createBlinkModeCommand(uint8_t mode);
  
  static AVRCommandData createFeatureMotorSpeedCommand(uint8_t featureNumber, uint8_t speedPercent);
  static AVRCommandData createFeatureMotorDirectionCommand(uint8_t featureNumber, uint8_t direction);
  static AVRCommandData createFeatureMotorTimeoutCommand(uint8_t featureNumber, uint8_t timeoutSeconds);
  static AVRCommandData createFeatureMotorTimeoutMillisecondsCommand(uint8_t featureNumber, uint16_t timeoutMilliseconds);
private:
  static String getPositionIdentifier(Position pos);
};

class AVRProtocol {
public:
  static void init();
  static void send(AVRCommandData command);
  static void send(std::vector<AVRCommandData> commands);
  static void ping();
};

#endif
