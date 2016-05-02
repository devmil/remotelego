#ifndef LEGOCONTROL_BLECOMMUNICATION_H_
#define LEGOCONTROL_BLECOMMUNICATION_H_

#include "Bluetooth.hpp"

#include "AVRCommunication.h"

//BLE configuration data
#define BLE_DEVICE_NAME                         "Lego RC"
#define EDDYSTONE_URL                           "www.google.de";

class SpeedCharacteristic : public BluetoothCharacteristic {
public:
    SpeedCharacteristic() 
    : BluetoothCharacteristic(std::string("8d8ba32b-96be-4590-910b-c756c5222c9f")) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Write, BluetoothCharacteristicProperty::WriteWithoutResponse };
    }
    virtual uint16_t getDataSize() {
      return 1;
    }
protected:
    virtual std::vector<uint8_t> onRead() {
      //NOOP
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid speed received!");
        return;
      }
      int8_t percent = data[0];
      uint8_t direction = percent < 0 ? 0 : 1;
      uint8_t speed = (uint8_t)abs(percent);
      String command = "\\cms=" + String(speed) + "|md=" + String(direction) + "/";
      AVR_sendCommands(command, 2);
    }
};

class SteerCharacteristic : public BluetoothCharacteristic {
public:
    SteerCharacteristic() 
    : BluetoothCharacteristic(std::string("7baf8dca-2bfc-47fb-af29-042fccc180eb")) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Write, BluetoothCharacteristicProperty::WriteWithoutResponse };
    }
    virtual uint16_t getDataSize() {
      return 1;
    }
protected:
    virtual std::vector<uint8_t> onRead() {
      //NOOP
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid steering received!");
        return;
      }
      int8_t angle = data[0]; //range = -90 - 90
      float fraction = (angle + 90.0f) / 180.0f;
      uint8_t percent = fabs(fraction) * 100u;
      String command = "\\csp=" + String(percent) + "/";
      AVR_sendCommands(command, 1);
    }
};

class LegoCarService : public BluetoothService {
public:
  LegoCarService() 
  : BluetoothService(std::string("40480f29-7bad-4ea5-8bf8-499405c9b324")) {
    addCharacteristic(std::make_shared<SpeedCharacteristic>());
    addCharacteristic(std::make_shared<SteerCharacteristic>());
  }
};

void BLE_configure();

#endif
