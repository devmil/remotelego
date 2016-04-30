//BLE configuration data
#define BLE_DEVICE_NAME                         "RB_Duo"
const char* DEVICE_NAME =                       "Lego RC";

const char* EDDYSTONE_URL =                     "www.google.de";

#define CHARACTERISTIC_SPEED_MAX_LEN            8
#define CHARACTERISTIC_STEER_MAX_LEN            8
#define MAX_DATA_LEN                            8

typedef struct {
  uint8_t guid[16];
  uint8_t data[MAX_DATA_LEN];
} BLECharacteristic;

//40480f29-7bad-4ea5-8bf8-499405c9b324
static uint8_t SERVICE_LEGOCAR_GUID[16] =       { 0x40, 0x48, 0x0f, 0x29, 0x7b, 0xad, 0x4e, 0xa5, 0x8b, 0xf8, 0x49, 0x94, 0x05, 0xc9, 0xb3, 0x24 };
//8d8ba32b-96be-4590-910b-c756c5222c9f
static uint8_t CHARACTERISTIC_SPEED_GUID[16] =  { 0x8d, 0x8b, 0xa3, 0x2b, 0x96, 0xbe, 0x45, 0x90, 0x91, 0x0b, 0xc7, 0x56, 0xc5, 0x22, 0x2c, 0x9f };
//7baf8dca-2bfc-47fb-af29-042fccc180eb
static uint8_t CHARACTERISTIC_STEER_GUID[16] =  { 0x7b, 0xaf, 0x8d, 0xca, 0x2b, 0xfc, 0x47, 0xfb, 0xaf, 0x29, 0x04, 0x2f, 0xcc, 0xc1, 0x80, 0xeb };

static uint8_t  BLE_appearance[2]    = {0x00, 0x02};
static uint8_t  BLE_change[2]        = {0x00, 0x00};
static uint8_t  BLE_conn_param[8]    = {0x28, 0x00, 0x90, 0x01, 0x00, 0x00, 0x90, 0x01};

static advParams_t BLE_adv_params;
static uint8_t BLE_adv_data[31];
static uint8_t BLE_adv_data_length = 0;

static advParams_t BLE_adv_params_eddystone;
static uint8_t BLE_adv_data_eddystone[31];
static uint8_t BLE_adv_data_eddystone_length = 0;


uint16_t s_characteristicSpeedHandle = 0;
uint8_t s_characteristicSpeedData[CHARACTERISTIC_SPEED_MAX_LEN] = { 0x00 };
uint16_t s_characteristicSteerHandle = 0;
uint8_t s_characteristicSteerData[CHARACTERISTIC_SPEED_MAX_LEN] = { 0x00 };


void BLE_onDeviceConnected(BLEStatus_t status, uint16_t handle) {
  switch (status){
      case BLE_STATUS_OK:
          Serial.println("Device connected!");
          AVR_sendCommands("\\csc=0x0000FF/", 1);
          break;
      default:
          Serial.println("Device almost connected!");
          break;
  }
}

void BLE_onDeviceDisconnected(uint16_t handle) {
  Serial.println("Disconnected.");
  AVR_stopAll();
  AVR_sendCommands("\\csc=0x00FF00/", 1);
}

uint16_t BLE_onDataRead(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size) {
  //Not supported yet
}

int BLE_onDataWrite(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  if(value_handle == s_characteristicSpeedHandle) {
    if(size != 1) {
      Serial.println("Invalid speed received!");
      return 1;
    }
    int8_t percent = (uint8_t)buffer[0];
    uint8_t direction = percent < 0 ? 0 : 1;
    uint8_t speed = (uint8_t)abs(percent);
    String command = "\\cms=" + String(speed) + "|md=" + String(direction) + "/";
    AVR_sendCommands(command, 2);
  } else if(value_handle == s_characteristicSteerHandle) {
  if(size != 1) {
      Serial.println("Invalid steering received!");
      return 1;
    }
    int8_t angle = (int8_t)buffer[0]; //range = -90 - 90
    float fraction = (angle + 90.0f) / 180.0f;
    uint8_t percent = fabs(fraction) * 100u;
    String command = "\\csp=" + String(percent) + "/";
    AVR_sendCommands(command, 1);
  }

  return 0;
}

void BLE_addAdvertisingDataByte(uint8_t* target, uint8_t* targetSize, uint8_t type,  uint8_t data) {
  BLE_addAdvertisingData(target, targetSize, type, &data, 1);
}

void BLE_addAdvertisingDataEddystone(uint8_t* target, uint8_t* targetSize, uint8_t type,  const char* url) {
  uint8_t data[31];
  data[0] = 0xAA; //Eddystone ID 1
  data[1] = 0xFE; //Eddystone ID 2
  data[2] = 0x10; //URL
  data[3] = 0xEB; //power
  data[4] = 0x02; //http
  uint8_t headerSize = 5;
  uint8_t dataLen = strlen(url) + headerSize;
  for(uint8_t i=0; i<dataLen - headerSize; i++) {
    if(i + headerSize < 31) {
      data[i + headerSize] = url[i];
    }
  }
  BLE_addAdvertisingData(target, targetSize, type, data, dataLen);  
}

void BLE_addAdvertisingDataString(uint8_t* target, uint8_t* targetSize, uint8_t type,  const char* str) {
  uint8_t data[31];
  uint8_t dataLen = strlen(str);
  for(uint8_t i=0; i<dataLen; i++) {
    data[i] = str[i];
  }
  BLE_addAdvertisingData(target, targetSize, type, data, dataLen);
}


void BLE_addAdvertisingDataInverted(uint8_t* target, uint8_t* targetSize, uint8_t type,  const uint8_t* data, uint8_t size) {
  uint8_t invertedData[31];
  for(int16_t i=size - 1; i>=0; i--) {
    invertedData[size - i - 1] = data[i];
  }
  BLE_addAdvertisingData(target, targetSize, type, invertedData, size);
}

void BLE_addAdvertisingData(uint8_t* target, uint8_t* targetSize, uint8_t type,  const uint8_t* data, uint8_t size) {
  int16_t realSize = size;
  if(*targetSize + size + 2 > 31) {
    realSize = 31 - 2 - *targetSize;
  }
  if(realSize < 0) {
    return;
  }
  target[*targetSize] = realSize + 1;
  *targetSize = *targetSize + 1;
  target[*targetSize] = type;
  *targetSize = *targetSize + 1;
  for(uint8_t i = 0; i<realSize; i++) {
    target[*targetSize] = data[i];
    *targetSize = *targetSize + 1;
  }
}

void BLE_advertiseServices(uint8_t isRestart) {
  ble.stopAdvertising();

  ble.setAdvParams(&BLE_adv_params);

  ble.setAdvData(BLE_adv_data_length, BLE_adv_data);
  
  if(!isRestart) {
    ble.startAdvertising();
  }
}

void BLE_advertiseEddystone(uint8_t isRestart) {
  ble.stopAdvertising();

  ble.setAdvParams(&BLE_adv_params_eddystone);

  ble.setAdvData(BLE_adv_data_eddystone_length, BLE_adv_data_eddystone);
  
  if(!isRestart) {
    ble.startAdvertising();
  }
}

void BLE_configure() {
  ble.init();
  ble.onConnectedCallback(BLE_onDeviceConnected);
  ble.onDisconnectedCallback(BLE_onDeviceDisconnected);
  ble.onDataReadCallback(BLE_onDataRead);
  ble.onDataWriteCallback(BLE_onDataWrite);

  //Default Services
  ble.addService(0x1800);
  ble.addCharacteristic(0x2A00, ATT_PROPERTY_READ, (uint8_t*)BLE_DEVICE_NAME, sizeof(BLE_DEVICE_NAME));
  ble.addCharacteristic(0x2A01, ATT_PROPERTY_READ, BLE_appearance, sizeof(BLE_appearance));
  ble.addCharacteristic(0x2A04, ATT_PROPERTY_READ, BLE_conn_param, sizeof(BLE_conn_param));
  ble.addService(0x1801);
  ble.addCharacteristic(0x2A05, ATT_PROPERTY_INDICATE, BLE_change, sizeof(BLE_change));

  //LegoCar service
  ble.addService(SERVICE_LEGOCAR_GUID);
  s_characteristicSpeedHandle = ble.addCharacteristicDynamic(CHARACTERISTIC_SPEED_GUID, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_WRITE, s_characteristicSpeedData, CHARACTERISTIC_SPEED_MAX_LEN);
  s_characteristicSteerHandle = ble.addCharacteristicDynamic(CHARACTERISTIC_STEER_GUID, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_WRITE, s_characteristicSteerData, CHARACTERISTIC_STEER_MAX_LEN);

  BLE_adv_params.adv_int_min = 0x00A0;
  BLE_adv_params.adv_int_max = 0x01A0;
  BLE_adv_params.adv_type    = 0;
  BLE_adv_params.dir_addr_type = 0;
  memset(BLE_adv_params.dir_addr,0,6);
  BLE_adv_params.channel_map = 0x07;
  BLE_adv_params.filter_policy = 0x00;

  //Type=1, data=0x06
  BLE_addAdvertisingDataByte(BLE_adv_data, &BLE_adv_data_length, 0x01, 0x06);
  //Type=8, Device name
  BLE_addAdvertisingDataString(BLE_adv_data, &BLE_adv_data_length, 0x08, DEVICE_NAME);
  //Type=6, Services
  BLE_addAdvertisingDataInverted(BLE_adv_data, &BLE_adv_data_length, 0x06, SERVICE_LEGOCAR_GUID, 16);

  BLE_adv_params_eddystone.adv_int_min = 0x00A0;
  BLE_adv_params_eddystone.adv_int_max = 0x01A0;
  BLE_adv_params_eddystone.adv_type    = 3; //Eddystone
  BLE_adv_params_eddystone.adv_type    = 0;
  BLE_adv_params_eddystone.dir_addr_type = 0;
  memset(BLE_adv_params_eddystone.dir_addr,0,6);
  BLE_adv_params_eddystone.channel_map = 0x07;
  BLE_adv_params_eddystone.filter_policy = 0x00;

  //Type=1, data=0x06
  BLE_addAdvertisingDataByte(BLE_adv_data_eddystone, &BLE_adv_data_eddystone_length, 0x01, 0x06);
  uint8_t eddystoneHeaderData[2] = { 0xAA, 0xFE };
  BLE_addAdvertisingData(BLE_adv_data_eddystone, &BLE_adv_data_eddystone_length, 0x03, eddystoneHeaderData, 2);
  BLE_addAdvertisingDataEddystone(BLE_adv_data_eddystone, &BLE_adv_data_eddystone_length, 0x16, EDDYSTONE_URL);

  BLE_advertiseServices(0);


  delay(100);

  AVR_sendCommands("\\csc=0x00FF00/", 1); //state = green
}
