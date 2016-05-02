#include "BLECommunication.h"

void BLE_configure() {
  Bluetooth::getInstance()->addService(std::make_shared<LegoCarService>());
  Bluetooth::getInstance()->setDeviceConnectedCallback([&](BLEStatus_t s, uint16_t h) {
        switch (s){
          case BLE_STATUS_OK:
              Serial.println("Device connected!");
              AVR_sendCommands("\\csc=0x0000FF/", 1);
              break;
          default:
              Serial.println("Device almost connected!");
              break;
      }
    });
  Bluetooth::getInstance()->setDeviceDisconnectedCallback([&](uint16_t h) {
      Serial.println("Disconnected.");
      AVR_stopAll();
      AVR_sendCommands("\\csc=0x00FF00/", 1);
    });
  Bluetooth::getInstance()->init(BLE_DEVICE_NAME);

  Bluetooth::getInstance()->advertiseServices();

  AVR_sendCommands("\\csc=0x00FF00/", 1); //state = green
}
