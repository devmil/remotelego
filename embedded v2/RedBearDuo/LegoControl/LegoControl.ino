int avrReset = D0;
int led = D7; 

#if defined(ARDUINO) 
SYSTEM_MODE(MANUAL);//do not connect to cloud
#else
SYSTEM_MODE(AUTOMATIC);//connect to cloud
#endif

uint16_t DELAY_MS = 500;
uint16_t s_failCount=0;

void setup() {

  pinMode(avrReset, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(avrReset, HIGH);

  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial1.setTimeout(100);
}

void sendCommands(String commands, uint8_t numOfCommands) {
  bool ok = false;
  uint8_t tries = 0;
  while(!ok) {
    tries++;
    if(tries >= 4) {
      Serial1.print("\r\n\r\n3 tries failed :(\r\n\r\n");
      s_failCount++;
      if(s_failCount >= 2) {
        digitalWrite(avrReset, LOW);
        delay(100);
        digitalWrite(avrReset, HIGH);
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
  s_failCount = 0;
}

void loop() {

  digitalWrite(led, HIGH);

  delay(DELAY_MS);

  digitalWrite(led, LOW);

  delay(DELAY_MS);

  sendCommands("\\csp=100|ms=40|md=1/", 3);
  
  delay(DELAY_MS);

  sendCommands("\\csp=80|ms=30|md=0/", 3);
  
  delay(DELAY_MS);

  sendCommands("\\csp=0|ms=100|md=0/", 3);

  delay(DELAY_MS);

  sendCommands("\\csp=20|ms=80|md=0/", 3);

  delay(DELAY_MS);

  sendCommands("\\csp=50|ms=0|md=0/", 3);
}


