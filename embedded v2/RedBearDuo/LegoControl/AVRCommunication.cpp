#include "AVRCommunication.h"

AVRCommandData AVRCommandFactory::createPingCommand() {
  return AVRCommandData("pp", "");
}
AVRCommandData AVRCommandFactory::createSetSteeringOffsetCommand(int8_t offsetAngle) {
  uint8_t percent = static_cast<uint8_t>(((offsetAngle) * 100) / 180);
  return AVRCommandData("sso", String(percent));
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

AVRCommandData AVRCommandFactory::createFrontHeadlightCommand(bool on) {
  return AVRCommandData("sfh", on ? "1" : "0");
}

AVRCommandData AVRCommandFactory::createRearLightCommand(bool on) {
  return AVRCommandData("srl", on ? "1" : "0");
}

AVRCommandData AVRCommandFactory::createFrontFoglightCommand(Position pos, bool on) {
  return AVRCommandData("sff" + getPositionIdentifier(pos), on ? "1" : "0");
}

AVRCommandData AVRCommandFactory::createReversingLightCommand(bool on) {
  return AVRCommandData("srel", on ? "1" : "0");
}

AVRCommandData AVRCommandFactory::createBlinkLightLeftCommand(bool on) {
  return AVRCommandData("sblll", on ? "1" : "0");
}

AVRCommandData AVRCommandFactory::createBlinkLightRightCommand(bool on) {
  return AVRCommandData("sbllr", on ? "1" : "0");  
}
  
AVRCommandData AVRCommandFactory::createFeatureMotorSpeedCommand(uint8_t featureNumber, uint8_t speedPercent) {
  return AVRCommandData("fm" + String(featureNumber, DEC) + "s", String(speedPercent, DEC));
}

AVRCommandData AVRCommandFactory::createFeatureMotorDirectionCommand(uint8_t featureNumber, uint8_t direction) {
  return AVRCommandData("fm" + String(featureNumber, DEC) + "d", String(direction, DEC));
}

AVRCommandData AVRCommandFactory::createFeatureMotorTimeoutCommand(uint8_t featureNumber, uint8_t timeoutSeconds) {
  return AVRCommandData("fm" + String(featureNumber, DEC) + "ts", String(timeoutSeconds, DEC));
}

AVRCommandData AVRCommandFactory::createFeatureMotorTimeoutMillisecondsCommand(uint8_t featureNumber, uint16_t timeoutMilliseconds) {
  return AVRCommandData("fm" + String(featureNumber, DEC) + "tms", String(timeoutMilliseconds, DEC));
}

String AVRCommandFactory::getPositionIdentifier(Position pos) {
  switch(pos) {
    case Position::Left:
      return "l";
    case Position::Right:
      return "r";
    case Position::Both:
      return "";
    default:
      return "";
  }
  return "";
}


void AVRProtocol::init() {
  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial1.setTimeout(100);  
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

  //Serial.println("Sending AVR command: " + commandString);

  String awaitedResult = "\\rnc=" + String(commands.size(), DEC) + "/";

  bool ok = false;
  uint8_t tries = 0;
  while(!ok) {
    tries++;
    //clear serial receive buffer
    int availableBytes = Serial1.available();
    if(availableBytes > 0) {
      std::vector<char> buff(availableBytes);
      Serial1.readBytes(&buff[0], availableBytes);
    }
    Serial1.println(commandString);
    Serial1.flush();
    String echo = Serial1.readStringUntil('\n');
    String result = Serial1.readStringUntil('\n');
    result.replace("\r", "");
    ok = result.equals(awaitedResult);
    if(!ok) {
      delay(20);
      if(tries >= 4) {
        Serial.print("\r\n\r\n3 tries failed :(\r\n\r\nAwaited: " + awaitedResult + "\r\nCommand: " + commandString + "\r\nEcho: " + echo + "\r\nResult: " + result);
        s_failCount++;
        return;
      }
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
