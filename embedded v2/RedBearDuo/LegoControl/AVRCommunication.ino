uint16_t s_AVR_failCount=0;

void AVR_configure() {
  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial1.setTimeout(100);  
}

void AVR_sendCommands(String commands, uint8_t numOfCommands) {
  bool ok = false;
  uint8_t tries = 0;
  while(!ok) {
    tries++;
    if(tries >= 4) {
      Serial1.print("\r\n\r\n3 tries failed :(\r\n\r\n");
      s_AVR_failCount++;
      if(s_AVR_failCount >= 2) {
//        digitalWrite(avrReset, LOW);
        delay(100);
//        digitalWrite(avrReset, HIGH);
      }
      return;
    }
    Serial1.print(commands);
    Serial1.flush();
    String result = Serial1.readStringUntil('/');
    ok = result.indexOf("\\rnc=" + String(numOfCommands)) >= 0;
    if(!ok) {
      delay(300);
    }
  }
  s_AVR_failCount = 0;
}

void AVR_stopAll() {
  AVR_sendCommands("\\cms=0|sp=50/", 2);
}

void AVR_ping() {
  AVR_sendCommands("\\cpp/", 1);
}
