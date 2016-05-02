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
      AVRProtocol::send(
        { 
          AVRCommandFactory::createMotorSpeedCommand(abs(percent)),
          AVRCommandFactory::createMotorDirectionCommand(percent > 0) 
        });
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
      AVRProtocol::send(AVRCommandFactory::createServoAngleCommand(angle));
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
