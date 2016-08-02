//RedBearDuo Arduino Board Plugin v 0.2.10
//RedBearDuo Firmware v 0.2.4

SYSTEM_MODE(MANUAL);//do not connect to cloud

#include "AVRCommunication.hpp"
#include "BLECommunication.hpp"
#include "NANOCommunication.hpp"
#include "LegoCarModel.hpp"

#include "DynamicProfile.hpp"

int led = D7; 

uint16_t DELAY_LOW_FREQUENCY_ACTION_MS = 500;

uint8_t s_state = 0;

uint64_t s_lastLowFrequencyActionMillis = 0;
LegoCarModel s_carModel;
DynamicProfile* s_carProfile;

void TEMP_setDynamicProfileContent(DynamicProfile* dynamicProfile, ICarProfile* effectiveProfile) {
  DynamicProfileData data;
  data.invertSteering = effectiveProfile->invertSteering();
  data.maxAnglePositive = effectiveProfile->getMaxSteeringAnglePositive();
  data.maxAngleNegative = effectiveProfile->getMaxSteeringAngleNegative();
  data.offsetAngle = effectiveProfile->getSteeringOffsetAngle();
  auto name = effectiveProfile->getDeviceName();
  for(int i=0; i<DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE; i++) {
    if(i<sizeof(name)) {
      data.name[i] = name.charAt(i);
    } else {
      data.name[i] = 0;
    }
  }
  data.hasMovingFrontLights = effectiveProfile->hasMovingFrontLightsFeature();
  data.hasTrunk = effectiveProfile->hasTrunkFeature();
  data.canBlink = effectiveProfile->hasBlinkFeature();

  dynamicProfile->setData(data);
}

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  while(!Serial) {
    delay(10);
  }
  //here the data gets loaded
  s_carProfile = new DynamicProfile(0);

  Serial.println("LegoControl: start");

  Serial.println("LegoControl: init AVR communication");
  AVRProtocol::init();

  Serial.println("LegoControl: init BLE service");
  BLE_configure(s_carModel, s_carProfile);

  Serial.println("LegoControl: init BLE Nano communication");
  BLENano::init();

  Serial.println("LegoControl: init Car model");
  s_carModel.init(s_carProfile);
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
    s_carModel.loopLowFrequency();
  
    s_state++;
    s_lastLowFrequencyActionMillis = currentMillis;
  }

  s_carModel.loop();

  delay(10);
}


