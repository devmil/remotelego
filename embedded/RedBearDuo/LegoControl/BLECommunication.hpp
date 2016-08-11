#ifndef LEGOCONTROL_BLECOMMUNICATION_H_
#define LEGOCONTROL_BLECOMMUNICATION_H_

#include <vector>

#include "Bluetooth.hpp"

#include "LegoCarModel.hpp"
#include "ICarProfile.hpp"
#include "DynamicProfile.hpp"

class SpeedCharacteristic : public BluetoothCharacteristic {
public:
    SpeedCharacteristic(LegoCarModel& model) 
    : BluetoothCharacteristic(String("8d8ba32b-96be-4590-910b-c756c5222c9f")),
      m_model(model){
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
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
    : BluetoothCharacteristic(String("7baf8dca-2bfc-47fb-af29-042fccc180eb")),
      m_model(model) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
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
    : BluetoothCharacteristic(String("e0af3340-022e-47e1-a263-d68887dc41d4")),
      m_model(model) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
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
    : BluetoothCharacteristic(String("fa10e4de-259e-4d23-9f59-45a9c66802ca")),
      m_model(model) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
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
    : BluetoothCharacteristic(String("aad03b81-f2ea-47db-ae1e-7c2f9e86e93e")),
      m_model(model) {
    }
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
      return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
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

class ConfigurationCharacteristic : public BluetoothCharacteristic {
public:
  ConfigurationCharacteristic(DynamicProfile* dynamicProfile)
  : BluetoothCharacteristic(String("2ac1fdb2-d971-4595-8e32-e8c5d80edf5f")),
    m_dynamicProfile(dynamicProfile) {
  }
  virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
    return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
  }
  virtual uint16_t getDataSize() {
    return m_dynamicProfile->getDataSize();
  }
protected:
    virtual std::vector<uint8_t> onRead() {
      std::vector<uint8_t> result;
      auto data = m_dynamicProfile->getData();
      uint8_t* castedData = (uint8_t*)&data;
      for(int i=0; i<m_dynamicProfile->getDataSize(); i++) {
        result.push_back(castedData[i]);
      }
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != getDataSize()) {
        Serial.println("Invalid configuration data received!");
        return;
      }
      Serial.println("Configuration data received...");
      DynamicProfileData profileData;
      uint8_t* castedData = (uint8_t*)&profileData;
      for(unsigned int i=0; i<data.size(); i++) {
        castedData[i] = data[i];
      }
      m_dynamicProfile->setData(profileData);
    }
private:
  DynamicProfile* m_dynamicProfile;
};

class NameConfigurationCharacteristic : public BluetoothCharacteristic {
public:
  NameConfigurationCharacteristic(DynamicProfile* dynamicProfile)
  : BluetoothCharacteristic(String("b394673e-dea0-4044-a189-86f1c85ce22e")),
    m_dynamicProfile(dynamicProfile) {
  }
  virtual std::vector<BluetoothCharacteristicProperty> getProperties() {
    return { BluetoothCharacteristicProperty::Read, BluetoothCharacteristicProperty::WriteWithoutResponse };
  }
  virtual uint16_t getDataSize() {
    return m_dynamicProfile->getNameDataSize();
  }
protected:
    virtual std::vector<uint8_t> onRead() {
      std::vector<uint8_t> result;
      auto data = m_dynamicProfile->getNameData();
      uint8_t* castedData = (uint8_t*)&data;
      for(int i=0; i<m_dynamicProfile->getNameDataSize(); i++) {
        result.push_back(castedData[i]);
      }
      return result;
    }
    virtual void onWrite(std::vector<uint8_t> data) {
      if(data.size() != getDataSize()) {
        Serial.println("Invalid name configuration data received!");
        return;
      }
      Serial.println("Name configuration data received...");
      DynamicProfileNameData profileNameData;
      uint8_t* castedData = (uint8_t*)&profileNameData;
      for(unsigned int i=0; i<data.size(); i++) {
        castedData[i] = data[i];
      }
      m_dynamicProfile->setNameData(profileNameData);
    }
private:
  DynamicProfile* m_dynamicProfile;
};


class LegoCarService : public BluetoothService {
public:
  LegoCarService(LegoCarModel& model, DynamicProfile* carProfile) 
  : BluetoothService(String("40480f29-7bad-4ea5-8bf8-499405c9b324")) {
    
    m_profile = carProfile;

    addCharacteristic(std::make_shared<SpeedCharacteristic>(model));
    addCharacteristic(std::make_shared<SteerCharacteristic>(model));

    m_usedTrunk = carProfile->hasTrunkFeature(); 
    if(carProfile->hasTrunkFeature()) {
      addCharacteristic(std::make_shared<TrunkCharacteristic>(model));
    }
    
    m_usedMFL = carProfile->hasMovingFrontLightsFeature(); 
    if(carProfile->hasMovingFrontLightsFeature()) {
      addCharacteristic(std::make_shared<MovableFrontLightCharacteristic>(model));
    }
    
    m_usedBlink = carProfile->hasBlinkFeature(); 
    if(carProfile->hasBlinkFeature()) {
      addCharacteristic(std::make_shared<BlinkCharacteristic>(model));
    }
    addCharacteristic(std::make_shared<ConfigurationCharacteristic>(carProfile));
    addCharacteristic(std::make_shared<NameConfigurationCharacteristic>(carProfile));
  }
private:
  DynamicProfile* m_profile;
  bool m_usedTrunk;
  bool m_usedMFL;
  bool m_usedBlink;
};

void BLE_configure(LegoCarModel& model, DynamicProfile* profile);

#endif
