#ifndef LEGOCONTROL_BLINKCONTROL_HPP_
#define LEGOCONTROL_BLINKCONTROL_HPP_

#include "Arduino.h"

enum class BlinkMode {
  Off   = 0,
  Right = 1,
  Left  = 2,
  Both  = 3,
};

class BlinkControl {
public:
  BlinkControl();
  ~BlinkControl();

  void setMode(BlinkMode mode);
  BlinkMode getMode();

  void loop();
  void init();
private:
  BlinkMode m_mode;
  uint64_t m_lastSwitchMillis;
  boolean m_currentlyActive;
  boolean m_isInitialized;

  static const uint16_t BLINK_INTERVAL_MS = 500;

  void resetInterval();
  void sendLedState();
};

#endif
