#if defined(ARDUINO) 
SYSTEM_MODE(MANUAL);//do not connect to cloud
#else
SYSTEM_MODE(AUTOMATIC);//connect to cloud
#endif

#include "AVRCommunication.h"
#include "BLECommunication.h"
#include "NANOCommunication.h"
#include "LegoCarModel.hpp"

int led = D7; 

uint16_t DELAY_MS = 500;

uint8_t s_state = 0;

LegoCarModel s_carModel;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  AVRProtocol::init();

  BLE_configure(s_carModel);

  BLENano::init();
}


void loop() {
  AVRProtocol::ping(); //this currently pings the AVR every 500ms as we have a 500ms loop delay. After 1s with no activity the AVR will stop any operation
  BLENano::ensureEddystoneUrl("https://www.devmil.de");

  if(s_state % 2 == 0) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  s_state++;
  delay(DELAY_MS);
}


