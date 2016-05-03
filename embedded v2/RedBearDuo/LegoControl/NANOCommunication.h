#ifndef LEGOCONTROL_NANOCOMMUNICATION_H_
#define LEGOCONTROL_NANOCOMMUNICATION_H_

#include "Arduino.h"

class BLENano {
public:
  static void init();

  static void ensureEddystoneUrl(String url);
private:
  static String s_lastUrlSentSuccessfully;

  static bool sendEddystoneUrl(String url);
};

#endif
