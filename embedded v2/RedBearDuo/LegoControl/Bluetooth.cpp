#include "Bluetooth.hpp"

static uint8_t  BLE_appearance[2]    = {0x00, 0x02};
static uint8_t  BLE_change[2]        = {0x00, 0x00};
static uint8_t  BLE_conn_param[8]    = {0x28, 0x00, 0x90, 0x01, 0x00, 0x00, 0x90, 0x01};

void addUUIDStringToByteVector(std::string uuid, std::vector<uint8_t>& target) {
  bool highByte = true;
  std::string hexVal;
  for(auto c : uuid) {
      if(c != '-') {
          if(highByte) {
            hexVal = c;
            highByte = false;
          } else {
            hexVal = hexVal + c;
            highByte = true;
            uint8_t byteVal = strtoul(hexVal.c_str(), 0, 16);
            target.push_back(byteVal);
          }
      }
  }
}

BluetoothCharacteristic::BluetoothCharacteristic(std::string uuid)
: m_uuid(uuid),
  m_uuidBytes() {
    addUUIDStringToByteVector(uuid, m_uuidBytes);
}

BluetoothCharacteristic::~BluetoothCharacteristic() {
}

void BluetoothCharacteristic::init() {
    m_data = std::vector<uint8_t>(getDataSize());
}

std::vector<uint8_t>& BluetoothCharacteristic::getDataRef() {
    return m_data;
}

std::vector<uint8_t>& BluetoothCharacteristic::getUUIDDataRef() {
  return m_uuidBytes;
}

int BluetoothCharacteristic::write(std::vector<uint8_t> data) {
    int count = data.size();
    onWrite(data);
    return count;   
}

std::vector<uint8_t> BluetoothCharacteristic::read() {
    auto result = onRead();
    
    return result;
}

BluetoothService::BluetoothService(std::string uuid)
: m_uuid(uuid),
  m_uuidBytes(),
  m_characteristics(),
  m_characteristicMap() {
    addUUIDStringToByteVector(uuid, m_uuidBytes);
}

BluetoothService::~BluetoothService() {    
}

void BluetoothService::addCharacteristic(BluetoothCharacteristic::Ptr characteristic) {
    m_characteristics.push_back(characteristic);
}

bool BluetoothService::hasCharacteristic(uint16_t handle) {
    return m_characteristicMap.find(handle) != m_characteristicMap.end();
}

BluetoothCharacteristic::Ptr BluetoothService::getCharacteristic(uint16_t handle) {
    auto iter = m_characteristicMap.find(handle);
    if(iter == m_characteristicMap.end()) {
        return 0;
    }
    return iter->second;
}

std::vector<uint8_t> BluetoothService::getUUIDData() {
  return m_uuidBytes;
}

void BluetoothService::init() {
    ble.addService(&m_uuidBytes[0]);
    for(auto characteristic : m_characteristics) {
        uint8_t properties = 0;
        for(auto prop : characteristic->getProperties()) {
            properties |= static_cast<uint8_t>(prop);
        }        
        uint16_t handle = ble.addCharacteristicDynamic(
            &characteristic->getUUIDDataRef()[0], 
            properties, 
            &characteristic->getDataRef()[0], 
            characteristic->getDataSize());
        m_characteristicMap.insert({ handle, characteristic });
    }
}

Bluetooth::Bluetooth()
: m_isInitialized(false),
  m_deviceName(""),
  m_advParamsServices(),
  m_advDataServices(),
  m_advParamsEddystone(),
  m_advDataEddystone(),
  m_deviceConnectedCallback(0),
  m_deviceDisconnectedCallback(0),
  m_services() {
}
Bluetooth::~Bluetooth() {
    
}

void Bluetooth::init(std::string deviceName) {
  m_deviceName = deviceName;
  ble.init();
  ble.onConnectedCallback(Bluetooth_onDeviceConnectedCallback);
  ble.onDisconnectedCallback(Bluetooth_onDeviceDisconnectedCallback);
  ble.onDataReadCallback(Bluetooth_onDataRead);
  ble.onDataWriteCallback(Buetooth_onDataWrite);

  //Default Services
  ble.addService(0x1800);
  ble.addCharacteristic(0x2A00, ATT_PROPERTY_READ, (uint8_t*)m_deviceName.c_str(), m_deviceName.length());
  ble.addCharacteristic(0x2A01, ATT_PROPERTY_READ, BLE_appearance, sizeof(BLE_appearance));
  ble.addCharacteristic(0x2A04, ATT_PROPERTY_READ, BLE_conn_param, sizeof(BLE_conn_param));
  ble.addService(0x1801);
  ble.addCharacteristic(0x2A05, ATT_PROPERTY_INDICATE, BLE_change, sizeof(BLE_change));

  for(auto service : m_services) {
      service->init();
  }

  m_advParamsServices.adv_int_min = 0x00A0;
  m_advParamsServices.adv_int_max = 0x01A0;
  m_advParamsServices.adv_type    = 0;
  m_advParamsServices.dir_addr_type = 0;
  memset(m_advParamsServices.dir_addr,0,6);
  m_advParamsServices.channel_map = 0x07;
  m_advParamsServices.filter_policy = 0x00;

  initServiceAdvertisingData(deviceName);

  m_advParamsEddystone.adv_int_min = 0x00A0;
  m_advParamsEddystone.adv_int_max = 0x01A0;
  m_advParamsEddystone.adv_type    = 3; //Eddystone
  m_advParamsEddystone.adv_type    = 0;
  m_advParamsEddystone.dir_addr_type = 0;
  memset(m_advParamsEddystone.dir_addr,0,6);
  m_advParamsEddystone.channel_map = 0x07;
  m_advParamsEddystone.filter_policy = 0x00;

  initEddystoneAdvertisingData("");
}

void Bluetooth::setDeviceConnectedCallback(DeviceConnectedCallback cb) {
    m_deviceConnectedCallback = cb;
}

void Bluetooth::setDeviceDisconnectedCallback(DeviceDisconnectedCallback cb) {
    m_deviceDisconnectedCallback = cb;
}

void Bluetooth::addService(BluetoothService::Ptr service) {
    m_services.push_back(service);
}

void Bluetooth::advertiseServices() {
  ble.setAdvParams(&m_advParamsServices);

  ble.setAdvData(m_advDataServices.size(), &m_advDataServices[0]);

  ble.startAdvertising();
}

void Bluetooth::advertiseEddystoneUrl(std::string url) {
  ble.setAdvParams(&m_advParamsEddystone);

  ble.setAdvData(m_advDataEddystone.size(), &m_advDataEddystone[0]);    

  ble.startAdvertising();
}

Bluetooth::Ptr Bluetooth::getInstance() {
  static Ptr instance = std::make_shared<Bluetooth>();
  return instance;
}

void Bluetooth::onDeviceConnected(BLEStatus_t status, uint16_t handle) {
  if(m_deviceConnectedCallback) {
    m_deviceConnectedCallback(status, handle);
  }
}

void Bluetooth::onDeviceDisconnected(uint16_t handle) {
  if(m_deviceDisconnectedCallback) {
    m_deviceDisconnectedCallback(handle);
  }    
}

uint16_t Bluetooth::onDataRead(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size) {
    for(auto service : m_services) {
        if(service->hasCharacteristic(value_handle)) {
            auto characteristic = service->getCharacteristic(value_handle);
            std::vector<uint8_t> data = characteristic->read();
            for(unsigned int i=0; i<data.size(); i++) {
                if(i < buffer_size - 1) {
                    buffer[i] = data[i];
                }
            }
        }
    }
}

int Bluetooth::onDataWrite(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
    for(auto service : m_services) {
        if(service->hasCharacteristic(value_handle)) {
            auto characteristic = service->getCharacteristic(value_handle);
            std::vector<uint8_t> data;
            for(int i=0; i<size; i++) {
                data.push_back(buffer[i]);
            }
            return characteristic->write(data);
        }
    }    
    return 0;
}

void Bluetooth::initServiceAdvertisingData(std::string deviceName) {
  //Type=1, data=0x06
  addAdvertisingDataByte(m_advDataServices, 0x01, 0x06);
  //Type=8, Device name
  addAdvertisingDataString(m_advDataServices, 0x08, deviceName);
  //Type=6, Services
  addAdvertisingDataInverted(m_advDataServices, 0x06, m_services[0]->getUUIDData());
}

void Bluetooth::initEddystoneAdvertisingData(std::string eddystoneUrl) {
  //Type=1, data=0x06
  addAdvertisingDataByte(m_advDataEddystone, 0x01, 0x06);
  std::vector<uint8_t> eddystoneHeaderData = { 0xAA, 0xFE };
  addAdvertisingData(m_advDataEddystone, 0x03, eddystoneHeaderData);
  addAdvertisingDataEddystone(m_advDataEddystone, 0x16, eddystoneUrl);
}

void Bluetooth::addAdvertisingDataByte(std::vector<uint8_t>& target, uint8_t type, uint8_t b) {
  std::vector<uint8_t> data;
  data.push_back(b);
  addAdvertisingData(target, type, data);
}

void Bluetooth::addAdvertisingDataString(std::vector<uint8_t>& target, uint8_t type, std::string str) {
  std::vector<uint8_t> data;
  for(auto c : str) {
    data.push_back(static_cast<uint8_t>(c));
  }
  addAdvertisingData(target, type, data);
}

void Bluetooth::addAdvertisingDataInverted(std::vector<uint8_t>& target, uint8_t type, std::vector<uint8_t> data) {
  std::vector<uint8_t> inverted;
  for(int i=data.size() - 1; i>=0; i--) {
    inverted.push_back(data[i]);
  }
  addAdvertisingData(target, type, inverted);
}

void Bluetooth::addAdvertisingDataEddystone(std::vector<uint8_t>& target, uint8_t type,  std::string url) {
  std::vector<uint8_t> data;
  data.push_back(0xAA); //Eddystone ID 1
  data.push_back(0xFE); //Eddystone ID 2
  data.push_back(0x10); //URL
  data.push_back(0xEB); //power
  data.push_back(0x02); //http

  //TODO: detect http and remove

  for(auto c : url) {
    if(data.size() <= 30) {
      data.push_back(static_cast<uint8_t>(c));
    }
  }

  addAdvertisingData(target, type, data);  
}


void Bluetooth::addAdvertisingData(std::vector<uint8_t>& target, uint8_t type, std::vector<uint8_t> data) {
  uint16_t dataSize = data.size();
  if(target.size() + 2 + dataSize > 31) {
    dataSize = 31 - 2 - target.size();
  }

  if(dataSize < 0) {
    return;
  }

  target.push_back(static_cast<uint8_t>(data.size() + 1));
  target.push_back(type);

  for(auto b : data) {
    if(target.size() < 31) {
      target.push_back(b);
    }
  }
}


void Bluetooth_onDeviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
  Bluetooth::getInstance()->onDeviceConnected(status, handle);
}

void Bluetooth_onDeviceDisconnectedCallback(uint16_t handle) {
    Bluetooth::getInstance()->onDeviceDisconnected(handle);
}

uint16_t Bluetooth_onDataRead(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size) {
    return Bluetooth::getInstance()->onDataRead(value_handle, buffer, buffer_size);
}

int Buetooth_onDataWrite(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
    return Bluetooth::getInstance()->onDataWrite(value_handle, buffer, size);    
}


