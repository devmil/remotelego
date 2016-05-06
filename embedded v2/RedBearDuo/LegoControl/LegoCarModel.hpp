#ifndef LEGOCONTROL_LEGOCARMODEL_H_
#define LEGOCONTROL_LEGOCARMODEL_H_

#include "Arduino.h"

enum class LightState {
  Off = 0,
  Normal = 1,
  FogLight = 2
};

enum class CarState {
  Booting,
  Ready,
  ClientConnected
};

class LegoCarModel {
public:
  LegoCarModel();
  virtual ~LegoCarModel();

  int8_t getMotorSpeedPercent();
  void setMotorSpeedPercent(int8_t percent);

  int8_t getSteeringDegrees();
  void setSteeringDegrees(int8_t degrees);

  LightState getLightState();
  void setLightState(LightState lightState);

  CarState getCarState();
  void setCarState(CarState state);

  void stopAll();
private:
  int8_t m_speedPercent;
  int8_t m_steeringDegrees;
  LightState m_lightState;
  CarState m_carState;

  uint32_t getStateColor();
  void sendLightState();
};

#endif
