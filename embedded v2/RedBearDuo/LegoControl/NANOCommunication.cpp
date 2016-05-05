#include "NANOCommunication.h"

#include <vector>

void BLENano::init() {
  Serial2.begin(115200);
  Serial2.setTimeout(300);
  while(!Serial2) {
    ;
  }
}

void BLENano::ensureEddystoneUrl(String url) {
  if(!s_lastUrlSentSuccessfully.equals(url)) {
    if(sendEddystoneUrl(url)) {
      s_lastUrlSentSuccessfully = url;
    }
  }
}

String BLENano::s_lastUrlSentSuccessfully = String("");

bool BLENano::sendEddystoneUrl(String url) {
  //clear serial receive buffer
  int availableBytes = Serial2.available();
  if(availableBytes > 0) {
    std::vector<char> buff(availableBytes);
    Serial2.readBytes(&buff[0], availableBytes);
  }
  String commandString = String("\\cec=") + url + String("/");
  Serial2.print(commandString);
  Serial2.flush();
  //TODO: await echo and then result
  String awaitedResult = String("\\rnc=1/");
  Serial.println(String("Sent: ") + commandString + String("awaiting ") + awaitedResult);
  String echo = Serial2.readStringUntil('\n');
  String result = Serial2.readStringUntil('\n');
  result.replace("\r", "");
  bool ok = result.startsWith(awaitedResult);
  if(!ok) {
    Serial.println("Result not OK. Echo was " + echo + "Result was " + result);
  }
  return ok;
}

