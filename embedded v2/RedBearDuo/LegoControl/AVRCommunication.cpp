#include "AVRCommunication.h"

AVRCommandData AVRCommandFactory::createPingCommand() {
  return AVRCommandData("pp", "");
}
AVRCommandData AVRCommandFactory::createMotorSpeedCommand(uint8_t speedPercent) {
  return AVRCommandData("ms", String(speedPercent));
}
AVRCommandData AVRCommandFactory::createMotorDirectionCommand(bool isForward) {
  return AVRCommandData("md", isForward ? "1" : "0");
}
AVRCommandData AVRCommandFactory::createServoAngleCommand(float angle) {
    uint8_t percent = static_cast<uint8_t>(((angle + 90) * 100) / 180); //-90 - 90 => 0 - 100
  return AVRCommandData("sp", String(percent));
}

AVRCommandData AVRCommandFactory::createStatusColorCommand(uint32_t color) {
  return AVRCommandData("sc", String("0x") + String(color, HEX));  
}

void AVRProtocol::init() {
  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial1.setTimeout(100);  
}

void AVRProtocol::stopAll() {
  std::vector<AVRCommandData> commands;
  commands.push_back(AVRCommandFactory::createMotorSpeedCommand(0));
  commands.push_back(AVRCommandFactory::createServoAngleCommand(0));
  send(commands);
}

void AVRProtocol::send(std::vector<AVRCommandData> commands) {
  static uint16_t s_failCount = 0;
  String commandString;

  commandString += String("\\c");
  bool first = true;
  for(auto cmd : commands) {
    if(!first) {
      commandString += String("|");
    }
    commandString += String(cmd.name) + String("=") + String(cmd.value);
    first = false;
  }
  commandString += String("/");

  String awaitedResult = String("\\rnc=") + String(commands.size(), DEC);

  bool ok = false;
  uint8_t tries = 0;
  while(!ok) {
    tries++;
    if(tries >= 4) {
      Serial1.print("\r\n\r\n3 tries failed :(\r\n\r\n");
      s_failCount++;
      if(s_failCount >= 2) {
//        digitalWrite(avrReset, LOW);
        delay(100);
//        digitalWrite(avrReset, HIGH);
      }
      return;
    }
    Serial1.flush();
    Serial1.print(commandString);
    String result = Serial1.readStringUntil('/');
    ok = result.indexOf(awaitedResult) >= 0;
    if(!ok) {
      delay(20);
    }
  }
  s_failCount = 0;
}

void AVRProtocol::send(AVRCommandData command) {
  std::vector<AVRCommandData> commands;
  commands.push_back(command);
  send(commands);
}

void AVRProtocol::ping() {
  send(AVRCommandFactory::createPingCommand());
}
