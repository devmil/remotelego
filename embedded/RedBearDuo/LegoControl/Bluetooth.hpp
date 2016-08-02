#ifndef LEGOCONTROL_BLUETOOTH_H_
#define LEGOCONTROL_BLUETOOTH_H_

#include <vector>
#include <map>
#include <functional>
#include <memory>

#include <Arduino.h>
#include "btstack_hal.h"
#include "spark_wiring_btstack.h"

enum class BluetoothCharacteristicProperty : uint8_t {
    Read = ATT_PROPERTY_READ,
    Write = ATT_PROPERTY_WRITE,
    WriteWithoutResponse = ATT_PROPERTY_WRITE_WITHOUT_RESPONSE
};

class BluetoothCharacteristic {
public:
    using Ptr=std::shared_ptr<BluetoothCharacteristic>; 
    BluetoothCharacteristic(String uuid);
    virtual ~BluetoothCharacteristic();
    
    void init();
    std::vector<uint8_t>& getDataRef();
    std::vector<uint8_t>& getUUIDDataRef();
    
    virtual int write(std::vector<uint8_t> data);
    virtual std::vector<uint8_t> read();
    
    virtual std::vector<BluetoothCharacteristicProperty> getProperties() = 0;
    virtual uint16_t getDataSize() = 0;
protected:
    virtual std::vector<uint8_t> onRead() = 0;
    virtual void onWrite(std::vector<uint8_t> data) = 0;
private:  
    String m_uuid;
    std::vector<uint8_t> m_uuidBytes; 
    std::vector<uint8_t> m_data;
};

class BluetoothService {
public:
    using Ptr=std::shared_ptr<BluetoothService>;
    BluetoothService(String uuid);
    virtual ~BluetoothService();
    void addCharacteristic(BluetoothCharacteristic::Ptr characteristic);

    bool hasCharacteristic(uint16_t handle);
    BluetoothCharacteristic::Ptr getCharacteristic(uint16_t handle);

    std::vector<uint8_t> getUUIDData();
    
    void init();
private:
    String m_uuid;
    std::vector<uint8_t> m_uuidBytes; 
    std::vector<BluetoothCharacteristic::Ptr> m_characteristics;
    std::map<uint16_t, BluetoothCharacteristic::Ptr> m_characteristicMap;
};

class Bluetooth {
public:
    using Ptr = std::shared_ptr<Bluetooth>;
    using DeviceConnectedCallback = std::function<void(BLEStatus_t, uint16_t)>;
    using DeviceDisconnectedCallback = std::function<void(uint16_t)>;
    Bluetooth();
    virtual ~Bluetooth();

    static void init(String deviceName);
    static void setDeviceConnectedCallback(DeviceConnectedCallback cb);
    static void setDeviceDisconnectedCallback(DeviceDisconnectedCallback cb);
    static void addService(BluetoothService::Ptr service);
    static void advertiseServices();

private:
    bool m_isInitialized;
    String m_deviceName;
    advParams_t m_advParamsServices;
    std::vector<uint8_t> m_advDataServices;
    std::vector<uint8_t> m_scanResponseDataServices;
    DeviceConnectedCallback m_deviceConnectedCallback;
    DeviceDisconnectedCallback m_deviceDisconnectedCallback;
    std::vector<BluetoothService::Ptr> m_services;

    static Bluetooth::Ptr getInstance();

    void init_internal(String deviceName);
    void setDeviceConnectedCallback_internal(DeviceConnectedCallback cb);
    void setDeviceDisconnectedCallback_internal(DeviceDisconnectedCallback cb);
    void addService_internal(BluetoothService::Ptr service);
    void advertiseServices_internal();

    void initServiceAdvertisingData(String deviceName);

    static void addAdvertisingDataByte(std::vector<uint8_t>& target, uint8_t type, uint8_t b);
    static void addAdvertisingDataString(std::vector<uint8_t>& target, uint8_t type, String str);
    static void addAdvertisingDataInverted(std::vector<uint8_t>& target, uint8_t type, std::vector<uint8_t> data);
    static void addAdvertisingData(std::vector<uint8_t>& target, uint8_t type, std::vector<uint8_t> data);

    //
    void onDeviceConnected(BLEStatus_t status, uint16_t handle);
    void onDeviceDisconnected(uint16_t handle);
    uint16_t onDataRead(uint16_t value_handle, uint8_t * buffer);
    int onDataWrite(uint16_t value_handle, uint8_t *buffer, uint16_t size);

    static void onDeviceConnectedCallback(BLEStatus_t status, uint16_t handle);
    static void onDeviceDisconnectedCallback(uint16_t handle);
    static uint16_t onDataReadCallback(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size);
    static int onDataWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size);
};

#endif
