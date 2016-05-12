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

enum class TrunkState {
  Unknown = 0,
  Closed = 1,
  Open = 2
};

enum class MovableFrontLightState {
  Unknown = 0,
  Hidden = 1,
  Active = 2
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

  TrunkState getTrunkState();
  void setTrunkState(TrunkState trunkState);

  MovableFrontLightState getMovableFrontLightState();
  void setMovableFrontLightState(MovableFrontLightState mflState);

  void stopAll();
private:
  int8_t m_speedPercent;
  int8_t m_steeringDegrees;
  LightState m_lightState;
  CarState m_carState;
  TrunkState m_trunkState;
  MovableFrontLightState m_mflState;

  uint32_t getStateColor();
  void sendLightState();
};

#endif
