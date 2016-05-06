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
uint8_t s_advertisingState = 1;
LegoCarModel s_carModel;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  AVRProtocol::init();

  BLE_configure(s_carModel);

  BLENano::init();
}


void loop() {
  AVRProtocol::ping(); //this currently pings the AVR every 500ms as we have a 500ms loop delay
  BLENano::ensureEddystoneUrl("https://www.devmil.de");
  //this doesn't work as switching the advertising frequently breaks both mechanisms: BLE clients can't connect and the physical web can't see the beacon
  if(s_advertisingState == 0) {
    //BLE_advertiseServices(1);    
  }
  if(s_advertisingState == 3) {
    //BLE_advertiseEddystone(1);
  }
  if(s_state % 2 == 0) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  s_state++;
  s_advertisingState++;

  if(s_advertisingState == 5) {
    s_advertisingState = 0;
  }

  delay(DELAY_MS);
}


