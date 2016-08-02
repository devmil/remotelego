#include "BLECommunication.hpp"

void BLE_configure(LegoCarModel& model, DynamicProfile* carProfile) {
  Bluetooth::addService(std::make_shared<LegoCarService>(model, carProfile));
  Bluetooth::setDeviceConnectedCallback([&](BLEStatus_t s, uint16_t h) {
        switch (s){
          case BLE_STATUS_OK:
              Serial.println("Device connected!");
              model.setCarState(CarState::ClientConnected);
              break;
          default:
              Serial.println("Device almost connected!");
              break;
      }
    });
  Bluetooth::setDeviceDisconnectedCallback([&](uint16_t h) {
      Serial.println("Disconnected.");
      model.stopAll();
      model.setCarState(CarState::Ready);
    });
  Bluetooth::init(carProfile->getDeviceName());

  Bluetooth::advertiseServices();

  model.setCarState(CarState::Ready);
}
