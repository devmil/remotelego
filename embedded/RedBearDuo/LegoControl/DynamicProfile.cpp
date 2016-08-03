#include "DynamicProfile.hpp"

#include <algorithm>

DynamicProfile::DynamicProfile(int eepromOffset) {
    m_eepromOffset = eepromOffset;
    m_data.invertSteering = false;
    m_data.maxAnglePositive = 90;
    m_data.maxAngleNegative = -90;
    m_data.offsetAngle = 0;
    m_data.hasMovingFrontLights = false;
    m_data.hasTrunk = false;
    m_data.canBlink = false;

    setNameFromString("not-yet-set");

    load();
}

DynamicProfile::~DynamicProfile() {

}

bool DynamicProfile::invertSteering() {
    return m_data.invertSteering;
}

int8_t DynamicProfile::getMaxSteeringAnglePositive() {
    return m_data.maxAnglePositive;
}

int8_t DynamicProfile::getMaxSteeringAngleNegative() {
    return m_data.maxAngleNegative;
}

int8_t DynamicProfile::getSteeringOffsetAngle() {
    return m_data.offsetAngle;
}

bool DynamicProfile::hasMovingFrontLightsFeature() {
    return m_data.hasMovingFrontLights;
}
    
bool DynamicProfile::hasTrunkFeature() {
    return m_data.hasTrunk;
}
    
bool DynamicProfile::hasBlinkFeature() {
    return m_data.canBlink;
}

String DynamicProfile::getDeviceName() {
    return getNameAsString();
}

void DynamicProfile::addListener(ICarProfileDataChangedListener* listener) {
    m_listener.push_back(listener);
}
void DynamicProfile::removeListener(ICarProfileDataChangedListener* listener) {
    auto iterator = std::find(m_listener.begin(), m_listener.end(), listener);
    if(iterator != m_listener.end()) {
        m_listener.erase(iterator);
    }
}


void DynamicProfile::setData(DynamicProfileData data) {
    m_data = data;

    printDataToLog();

    save();

    for(const auto& listener : m_listener) {
        listener->onDataChanged();
    }
}

DynamicProfileData DynamicProfile::getData() {
    return m_data;
}

uint16_t DynamicProfile::getDataSize() {
    return sizeof(DynamicProfileData);
}

uint16_t DynamicProfile::getNameDataSize() {
    return DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE;
}

DynamicProfileNameData DynamicProfile::getNameData() {
    return m_nameData;
}

void DynamicProfile::setNameData(DynamicProfileNameData nameData) {

    m_nameData = nameData;
    
    printDataToLog();

    save();

    for(const auto& listener : m_listener) {
        listener->onDataChanged();
    }
}

void DynamicProfile::load() {
    //check if there is any value
    if(EEPROM.read(m_eepromOffset) == 0xFF) {
        Serial.println("found nothing in EEProm, saving defaults");
        save();
    }
    uint8_t testBytes[2];
    testBytes[0] = EEPROM.read(m_eepromOffset + sizeof(DynamicProfileData) + sizeof(DynamicProfileNameData) + 0);
    testBytes[1] = EEPROM.read(m_eepromOffset + sizeof(DynamicProfileData) + sizeof(DynamicProfileNameData) + 1);
    if(testBytes[0] != 0xBE || testBytes[1] != 0xEF) {
        Serial.println("Test-bytes mismatch, saving defaults");
        save();
    }
    Serial.println("loading data from EEPROM");
    int offset = m_eepromOffset;
    EEPROM.get(offset, m_data);
    offset += sizeof(DynamicProfileData);
    EEPROM.get(offset, m_nameData);
    Serial.println("Name = " + String(getNameAsString()));
}

void DynamicProfile::save() {
    Serial.println("saving data to EEPROM");
    Serial.println("Name = " + getNameAsString());
    uint16_t offset = m_eepromOffset;
    EEPROM.put(offset, m_data);
    offset += sizeof(DynamicProfileData);
    EEPROM.put(offset, m_nameData);
    offset += sizeof(DynamicProfileNameData);
    EEPROM.update(offset, 0xBE);
    offset += 1;
    EEPROM.update(offset, 0xEF);
}

void DynamicProfile::printDataToLog() {
    Serial.println("Setting configuration data:");
    Serial.println("Invert steering = " + String(m_data.invertSteering));
    Serial.println("Max Positive    = " + String(m_data.maxAnglePositive));
    Serial.println("Max Negative    = " + String(m_data.maxAngleNegative));
    Serial.println("Offset          = " + String(m_data.offsetAngle));
    Serial.println("Name            = " + getNameAsString());
    Serial.println("Moving Lights   = " + String(m_data.hasMovingFrontLights));
    Serial.println("Trunk           = " + String(m_data.hasTrunk));
    Serial.println("Blink           = " + String(m_data.canBlink));
}

void DynamicProfile::setNameFromString(String name) {
    for(int i=0; i<DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE; i++) {
        if(i < name.length()) {
            m_nameData.name[i] = name.charAt(i);
        } else {
            m_nameData.name[i] = 0;
        }
    }
}

String DynamicProfile::getNameAsString() {
    char tmpName[DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE + 1];
    for(int i=0; i<DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE; i++) {
        tmpName[i] = m_nameData.name[i];
    }
    tmpName[DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE] = 0;
    return String(tmpName);
}
