#ifndef LEGOCONTROL_BLECOMMUNICATION_H_
#define LEGOCONTROL_BLECOMMUNICATION_H_

#include "Bluetooth.hpp"

#include "LegoCarModel.hpp"

//BLE configuration data
#define BLE_DEVICE_NAME                         "Lego RC"
#define EDDYSTONE_URL                           "www.google.de";

class SpeedCharacteristic : public BluetoothCharacteristic {
public:
    SpeedCharacteristic(LegoCarModel& model) 
    : BluetoothCharacteristic(std::string("8d8ba32b-96be-4590-910b-c756c5222c9f")),
      m_model(model){
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::Write, BluetoothCharacteristicProperty::WriteWithoutResponse };
    }
    virtual uint16_t getDataSize() {
      return 1;
    }
protected:
    virtual std::vector<uint8_t> onRead() {
      std::vector<uint8_t> result;
      result.push_back(m_model.getMotorSpeedPercent());
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid speed received!");
        return;
      }
      int8_t percent = data[0];
      m_model.setMotorSpeedPercent(percent);
    }
private:
  LegoCarModel& m_model;
};

class SteerCharacteristic : public BluetoothCharacteristic {
public:
    SteerCharacteristic(LegoCarModel& model) 
    : BluetoothCharacteristic(std::string("7baf8dca-2bfc-47fb-af29-042fccc180eb")),
      m_model(model) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::Write, BluetoothCharacteristicProperty::WriteWithoutResponse };
    }
    virtual uint16_t getDataSize() {
      return 1;
    }
protected:
    virtual std::vector<uint8_t> onRead() {
      std::vector<uint8_t> result;
      result.push_back(m_model.getSteeringDegrees());
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid steering received!");
        return;
      }
      int8_t angle = data[0]; //range = -90 - 90
      m_model.setSteeringDegrees(angle);
    }
private:
  LegoCarModel& m_model;
};

class LegoCarService : public BluetoothService {
public:
  LegoCarService(LegoCarModel& model) 
  : BluetoothService(std::string("40480f29-7bad-4ea5-8bf8-499405c9b324")) {
    addCharacteristic(std::make_shared<SpeedCharacteristic>(model));
    addCharacteristic(std::make_shared<SteerCharacteristic>(model));
  }
};

void BLE_configure(LegoCarModel& model);

#endif
