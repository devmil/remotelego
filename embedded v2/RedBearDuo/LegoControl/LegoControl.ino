//RedBearDuo Arduino Board Plugin v 0.2.7
//RedBearDuo Firmware v 0.2.3

SYSTEM_MODE(MANUAL);//do not connect to cloud

#include "AVRCommunication.h"
#include "BLECommunication.h"
#include "NANOCommunication.h"
#include "LegoCarModel.hpp"

int led = D7; 

uint16_t DELAY_LOW_FREQUENCY_ACTION_MS = 500;

uint8_t s_state = 0;

uint64_t s_lastLowFrequencyActionMillis = 0;
LegoCarModel s_carModel;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  while(!Serial) {
    delay(10);
  }
  Serial.println("LegoControl: start");

  Serial.println("LegoControl: init AVR communication");
  AVRProtocol::init();

  Serial.println("LegoControl: init BLE service");
  BLE_configure(s_carModel);

  Serial.println("LegoControl: init BLE Nano communication");
  BLENano::init();

  Serial.println("LegoControl: init Car model");
  s_carModel.init();
}


void loop() {
  uint64_t currentMillis = millis();
  if(currentMillis - s_lastLowFrequencyActionMillis > DELAY_LOW_FREQUENCY_ACTION_MS) {
    AVRProtocol::ping(); //this currently pings the AVR every 500ms
    if(s_state % 2 == 0) {
      digitalWrite(led, HIGH);
    } else {
      digitalWrite(led, LOW);
    }
  
    s_state++;
    s_lastLowFrequencyActionMillis = currentMillis;
  }

  s_carModel.loop();

  delay(10);
}


