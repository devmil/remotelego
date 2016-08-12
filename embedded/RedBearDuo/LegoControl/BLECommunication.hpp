#ifndef LEGOCONTROL_BLECOMMUNICATION_H_
#define LEGOCONTROL_BLECOMMUNICATION_H_

#include <vector>

#include "Bluetooth.hpp"

#include "LegoCarModel.hpp"
#include "ICarProfile.hpp"
#include "DynamicProfile.hpp"

class SpeedCharacteristic : public BluetoothCharacteristic {
public:
    SpeedCharacteristic(LegoCarModel& model);
    virtual std::vector<BluetoothCharacteristicProperty> getProperties();
    virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  LegoCarModel& m_model;
};

class SteerCharacteristic : public BluetoothCharacteristic {
public:
    SteerCharacteristic(LegoCarModel& model);
    virtual std::vector<BluetoothCharacteristicProperty> getProperties();
    virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  LegoCarModel& m_model;
};

class TrunkCharacteristic : public BluetoothCharacteristic {
public:
    TrunkCharacteristic(LegoCarModel& model);
    virtual std::vector<BluetoothCharacteristicProperty> getProperties();
    virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  LegoCarModel& m_model;
};

class MovableFrontLightCharacteristic : public BluetoothCharacteristic {
public:
    MovableFrontLightCharacteristic(LegoCarModel& model);
    virtual std::vector<BluetoothCharacteristicProperty> getProperties();
    virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  LegoCarModel& m_model;
};

class BlinkCharacteristic : public BluetoothCharacteristic {
public:
    BlinkCharacteristic(LegoCarModel& model);
    virtual std::vector<BluetoothCharacteristicProperty> getProperties();
    virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  LegoCarModel& m_model;
};

class ConfigurationCharacteristic : public BluetoothCharacteristic {
public:
  ConfigurationCharacteristic(DynamicProfile* dynamicProfile);
  virtual std::vector<BluetoothCharacteristicProperty> getProperties();
  virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  DynamicProfile* m_dynamicProfile;
};

class NameConfigurationCharacteristic : public BluetoothCharacteristic {
public:
  NameConfigurationCharacteristic(DynamicProfile* dynamicProfile);
  virtual std::vector<BluetoothCharacteristicProperty> getProperties();
  virtual uint16_t getDataSize();
protected:
    virtual std::vector<uint8_t> onRead();
    virtual void onWrite(std::vector<uint8_t> data);
private:
  DynamicProfile* m_dynamicProfile;
};


class LegoCarService : public BluetoothService {
public:
  LegoCarService(LegoCarModel& model, DynamicProfile* carProfile);
private:
  DynamicProfile* m_profile;
  bool m_usedTrunk;
  bool m_usedMFL;
  bool m_usedBlink;
};

void BLE_configure(LegoCarModel& model, DynamicProfile* profile);

#endif
