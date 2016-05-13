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

class TrunkCharacteristic : public BluetoothCharacteristic {
public:
    TrunkCharacteristic(LegoCarModel& model) 
    : BluetoothCharacteristic(std::string("e0af3340-022e-47e1-a263-d68887dc41d4")),
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
      result.push_back((uint8_t)m_model.getTrunkState());
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid trunk state received!");
        return;
      }
      int8_t trunkState = data[0]; //range = 0 - 2
      m_model.setTrunkState((TrunkState)trunkState);
    }
private:
  LegoCarModel& m_model;
};

class MovableFrontLightCharacteristic : public BluetoothCharacteristic {
public:
    MovableFrontLightCharacteristic(LegoCarModel& model) 
    : BluetoothCharacteristic(std::string("fa10e4de-259e-4d23-9f59-45a9c66802ca")),
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
      result.push_back((uint8_t)m_model.getMovableFrontLightState());
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid movable front light state received!");
        return;
      }
      int8_t mflState = data[0]; //range = 0 - 2
      m_model.setMovableFrontLightState((MovableFrontLightState)mflState);
    }
private:
  LegoCarModel& m_model;
};

class BlinkCharacteristic : public BluetoothCharacteristic {
public:
    BlinkCharacteristic(LegoCarModel& model) 
    : BluetoothCharacteristic(std::string("aad03b81-f2ea-47db-ae1e-7c2f9e86e93e")),
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
      result.push_back((uint8_t)m_model.getBlinkMode());
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != 1) {
        Serial.println("Invalid blink state received!");
        return;
      }
      int8_t blinkMode = data[0]; //range = 0 - 3
      m_model.setBlinkMode((BlinkMode)blinkMode);
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
    addCharacteristic(std::make_shared<TrunkCharacteristic>(model));
    addCharacteristic(std::make_shared<MovableFrontLightCharacteristic>(model));
    addCharacteristic(std::make_shared<BlinkCharacteristic>(model));
  }
};

void BLE_configure(LegoCarModel& model);

#endif
