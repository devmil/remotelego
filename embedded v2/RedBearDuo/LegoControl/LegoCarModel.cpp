#include "LegoCarModel.hpp"

#include "AVRCommunication.h"

LegoCarModel::LegoCarModel() {
  m_speedPercent = 0;
  m_steeringDegrees = 0;
  m_lightState = LightState::Normal;
}

LegoCarModel::~LegoCarModel() {
}

int8_t LegoCarModel::getMotorSpeedPercent() {
  return m_speedPercent;
}

void LegoCarModel::setMotorSpeedPercent(int8_t percent) {
  int signBefore = 1;
  if(m_speedPercent != 0) { 
    signBefore = m_speedPercent / abs(m_speedPercent);
  }
  int signAfter = 1;
  if(percent != 0) {
    signAfter = percent / abs(percent);
  }
  
  m_speedPercent = percent;
  AVRProtocol::send(
  { 
    AVRCommandFactory::createMotorSpeedCommand(abs(percent)),
    AVRCommandFactory::createMotorDirectionCommand(percent > 0) 
  });
  if(signBefore != signAfter) {
    sendLightState();
  }
}

int8_t LegoCarModel::getSteeringDegrees() {
  return m_steeringDegrees;
}

void LegoCarModel::setSteeringDegrees(int8_t degrees) {
  m_steeringDegrees = degrees;
  AVRProtocol::send(AVRCommandFactory::createServoAngleCommand(degrees));
}

LightState LegoCarModel::getLightState() {
  return m_lightState;
}

void LegoCarModel::setLightState(LightState lightState) {
  m_lightState = lightState;
  sendLightState();
}

CarState LegoCarModel::getCarState() {
  return m_carState;
}

void LegoCarModel::setCarState(CarState state) {
  m_carState = state;
  AVRProtocol::send(AVRCommandFactory::createStatusColorCommand(getStateColor()));
}

void LegoCarModel::stopAll() {
  setSteeringDegrees(0);
  setMotorSpeedPercent(0);
}

uint32_t LegoCarModel::getStateColor() {
  switch(m_carState) {
    case CarState::Booting:
      return 0xFF0000;
    case CarState::Ready:
      return 0x00FF00;
    case CarState::ClientConnected:
      return 0x0000FF;
    default:
      return 0xFF0000;
  }
}

void LegoCarModel::sendLightState() {
  bool ledsFrontOn = false;
  bool ledsBackOn = false;
  bool ledsFogOn = false;
  bool ledsReverseOn = false;

  ledsReverseOn = m_speedPercent < 0;
  if(m_lightState != LightState::Off) {
      ledsFrontOn = true;
      ledsBackOn = true;
      ledsFogOn = m_lightState == LightState::FogLight;
  }

  std::vector<AVRCommandData> commands;
  commands.push_back(AVRCommandFactory::createFrontHeadlightCommand(Position::Both, ledsFrontOn));
  commands.push_back(AVRCommandFactory::createRearLightCommand(Position::Both, ledsBackOn));
  commands.push_back(AVRCommandFactory::createFrontFoglightCommand(Position::Both, ledsFogOn));
  commands.push_back(AVRCommandFactory::createReversingLightCommand(Position::Both, ledsReverseOn));

  AVRProtocol::send(commands);
}

