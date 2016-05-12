#include "LegoCarModel.hpp"

#include "AVRCommunication.h"

LegoCarModel::LegoCarModel() {
  m_speedPercent = 0;
  m_steeringDegrees = 0;
  m_lightState = LightState::Normal;
  m_trunkState = TrunkState::Unknown;
  m_mflState = MovableFrontLightState::Unknown;
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

TrunkState LegoCarModel::getTrunkState() {
  return m_trunkState;
}

void LegoCarModel::setTrunkState(TrunkState trunkState) {
  if(trunkState == m_trunkState || trunkState == TrunkState::Unknown) {
    return;
  }
  m_trunkState = trunkState;

  uint8_t featureNumber = 1;
  uint8_t direction = 0;
  uint8_t speedPercent = 0;
  uint8_t timeoutSeconds = 0;

  speedPercent = 30;
  timeoutSeconds = 2;
  if(m_trunkState == TrunkState::Closed) {
    direction = 0;
  } else if(trunkState == TrunkState::Open) {
    direction = 1;
  }

  std::vector<AVRCommandData> commands;
  commands.push_back(AVRCommandFactory::createFeatureMotorSpeedCommand(featureNumber, speedPercent));
  commands.push_back(AVRCommandFactory::createFeatureMotorDirectionCommand(featureNumber, direction));
  commands.push_back(AVRCommandFactory::createFeatureMotorTimeoutCommand(featureNumber, timeoutSeconds));

  AVRProtocol::send(commands);
}

MovableFrontLightState LegoCarModel::getMovableFrontLightState() {
  return m_mflState;
}

void LegoCarModel::setMovableFrontLightState(MovableFrontLightState mflState) {
  if(m_mflState == mflState || mflState == MovableFrontLightState::Unknown) {
    return;
  }
  m_mflState = mflState;

  uint8_t featureNumber = 2;
  uint8_t direction = 0;
  uint8_t speedPercent = 0;
  uint8_t timeoutSeconds = 0;

  speedPercent = 70;
  timeoutSeconds = 3;
  if(m_mflState == MovableFrontLightState::Hidden) {
    direction = 0;
  } else if(m_mflState == MovableFrontLightState::Active) {
    direction = 1;
  }

  std::vector<AVRCommandData> commands;
  commands.push_back(AVRCommandFactory::createFeatureMotorSpeedCommand(featureNumber, speedPercent));
  commands.push_back(AVRCommandFactory::createFeatureMotorDirectionCommand(featureNumber, direction));
  commands.push_back(AVRCommandFactory::createFeatureMotorTimeoutCommand(featureNumber, timeoutSeconds));

  AVRProtocol::send(commands);
}

