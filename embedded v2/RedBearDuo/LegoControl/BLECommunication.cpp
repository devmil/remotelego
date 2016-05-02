#include "BLECommunication.h"

void BLE_configure() {
  Bluetooth::getInstance()->addService(std::make_shared<LegoCarService>());
  Bluetooth::getInstance()->setDeviceConnectedCallback([&](BLEStatus_t s, uint16_t h) {
        switch (s){
          case BLE_STATUS_OK:
              Serial.println("Device connected!");
              AVRProtocol::send(AVRCommandFactory::createStatusColorCommand(0x0000FF));
              break;
          default:
              Serial.println("Device almost connected!");
              break;
      }
    });
  Bluetooth::getInstance()->setDeviceDisconnectedCallback([&](uint16_t h) {
      Serial.println("Disconnected.");
      AVRProtocol::stopAll();
      AVRProtocol::send(AVRCommandFactory::createStatusColorCommand(0x00FF00));
    });
  Bluetooth::getInstance()->init(BLE_DEVICE_NAME);

  Bluetooth::getInstance()->advertiseServices();

  AVRProtocol::send(AVRCommandFactory::createStatusColorCommand(0x00FF00));
}
