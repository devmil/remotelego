#include "BlinkControl.hpp"

#include "AVRCommunication.hpp"

BlinkControl::BlinkControl() {
  m_isInitialized = false;
  m_mode = BlinkMode::Off;
  resetInterval();
}

BlinkControl::~BlinkControl() {
  
}

void BlinkControl::setMode(BlinkMode mode) {
  if(m_mode == mode) {
    return;  
  }
  m_mode = mode;
  resetInterval();
}

BlinkMode BlinkControl::getMode() {
  return m_mode;
}

void BlinkControl::loop() {
  if(m_mode == BlinkMode::Off) {
    return;
  }
  uint64_t currentMillis = millis();
  if(currentMillis - m_lastSwitchMillis > BLINK_INTERVAL_MS) {
    m_lastSwitchMillis = currentMillis;
    m_currentlyActive = !m_currentlyActive;
    sendLedState();
  }
}

void BlinkControl::init() {
  m_isInitialized = true;
  sendLedState();
}

void BlinkControl::resetInterval() {
  m_lastSwitchMillis = millis();
  m_currentlyActive = false;
  sendLedState();
}

void BlinkControl::sendLedState() {
  if(!m_isInitialized) {
    return;
  }
  uint8_t valueLeft = 0;
  uint8_t valueRight = 0;
  if(m_currentlyActive) {
    switch(m_mode) {
      case BlinkMode::Right:
        valueRight = 1;
        break;
      case BlinkMode::Left:
        valueLeft = 1;
        break;
      case BlinkMode::Both:
        valueRight = 1;
        valueLeft = 1;
        break;
      default:
        break;
    }
  }
  
  std::vector<AVRCommandData> commands;
  commands.push_back(AVRCommandFactory::createBlinkLightLeftCommand(valueLeft));
  commands.push_back(AVRCommandFactory::createBlinkLightRightCommand(valueRight));

  AVRProtocol::send(commands);
}
