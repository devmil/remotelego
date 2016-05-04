#include <BLE_API.h>

BLEDevice ble;

void initBle() {
  //Close peripheral power
  NRF_POWER->DCDCEN = 0x00000001;
  NRF_TIMER1->POWER = 0;
  NRF_TIMER2->POWER = 0;
  NRF_WDT->POWER = 0;
  NRF_TEMP->POWER = 0;
  //NRF_UART0->POWER = 0;

  ble.init();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {
    delay(10);
  }
  Serial.setTimeout(300);
  initBle();
  Serial.println("EdisonBroadcaster ready!");
}

const static uint8_t EDDYSTONE_FLAGS_DATA[] = { 0x06 };
const static uint8_t EDDYSTONE_HEADER_DATA[] = { 0xAA, 0xFE };
static uint8_t EDDYSTONE_SERVICE_DATA[31];

struct SchemeMapping {
  SchemeMapping(String pScheme, uint8_t pFlag) {
    flag = pFlag;
    scheme = pScheme;
  }
  uint8_t flag;
  String scheme;
};

const static SchemeMapping SCHEME_MAPPINGS[] = {
  //SchemeMapping(String("http://www."), 0x00),
  //SchemeMapping(String("https://www."), 0x01),
  SchemeMapping(String("http://"), 0x02),
  SchemeMapping(String("https://"), 0x03)
};

bool advertiseEddystone(String url) {
  uint8_t urlSchema = 0x02; //http
  for(int i=0; i<sizeof(SCHEME_MAPPINGS); i++) {
    String scheme = SCHEME_MAPPINGS[i].scheme;
    uint8_t flag = SCHEME_MAPPINGS[i].flag;
    if(url.startsWith(scheme)) {
      urlSchema = flag;
      url = url.substring(scheme.length(), url.length());
      //Serial.println(String("Scheme found: ") + scheme + ", setting flag to: " + String(urlSchema, HEX) + String(", Rest of the URL = ") + url);
      break;
    }
  }
  
  ble.stopAdvertising();

  uint8_t serviceDataIndex = 0;

  EDDYSTONE_SERVICE_DATA[serviceDataIndex++] = 0xAA; //Eddystone ID 1
  EDDYSTONE_SERVICE_DATA[serviceDataIndex++] = 0xFE; //Eddystone ID 2
  EDDYSTONE_SERVICE_DATA[serviceDataIndex++] = 0x10; //URL
  EDDYSTONE_SERVICE_DATA[serviceDataIndex++] = 0xEB; //power
  EDDYSTONE_SERVICE_DATA[serviceDataIndex++] = urlSchema;

  char urlChars[31];
  url.toCharArray(urlChars, sizeof(urlChars));

  for(int i=0; i<strlen(urlChars); i++) {
    if(serviceDataIndex < 30) {
      EDDYSTONE_SERVICE_DATA[serviceDataIndex++] = (uint8_t)urlChars[i];
    }
  }

  uint32_t errorCode = 0;

  ble.clearAdvertisingPayload();

//  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
  errorCode = errorCode | ble.accumulateAdvertisingPayload(GapAdvertisingData::FLAGS, EDDYSTONE_FLAGS_DATA /* 0x01 */, sizeof(EDDYSTONE_FLAGS_DATA));
  errorCode = errorCode | ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS /* 0x03 */, EDDYSTONE_HEADER_DATA, sizeof(EDDYSTONE_HEADER_DATA));
  errorCode = errorCode | ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA /* 0x16 */, EDDYSTONE_SERVICE_DATA, serviceDataIndex);
  
  ble.setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
  ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */

  if(errorCode == 0) {
    errorCode = errorCode | ble.startAdvertising();
  }

  return errorCode == 0;
}

const String EDISON_URL_COMMAND = "ec";

bool handleCommand(String command, String param) {
  if(command.equals(EDISON_URL_COMMAND)) {
    if(advertiseEddystone(param)) {
      Serial.println("\\rnc=1/");
    } else {
      Serial.println("\\rnc=0/");
    }
  }
}

void loop() {
  if(Serial.available() > 0) {
     String input = Serial.readStringUntil('\n');
     input.replace("\r", "");
     if(input.startsWith("\\c")) {
      String commandString = input.substring(2, input.length());
      int equalsIndex = commandString.indexOf("=");
      if(equalsIndex > 0 && equalsIndex < commandString.length()) {
        String command = commandString.substring(0, equalsIndex);
        String param = commandString.substring(equalsIndex + 1, commandString.length());
        handleCommand(command, param);
      }
     }
  }
}
