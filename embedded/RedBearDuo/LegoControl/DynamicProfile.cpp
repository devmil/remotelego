#include "DynamicProfile.hpp"

#include <algorithm>

DynamicProfile::DynamicProfile(int eepromOffset) {
    m_eepromOffset = eepromOffset;
    m_data.invertSteering = false;
    m_data.maxAnglePositive = 90;
    m_data.maxAngleNegative = -90;
    m_data.offsetAngle = 0;
    auto initName = "Not-yet-set";
    for(int i=0; i<DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE; i++) {
        if(i < sizeof(initName)) {
            m_data.name[i] = initName[i];
        } else {
            m_data.name[i] = 0;
        }
    }
    m_data.hasMovingFrontLights = false;
    m_data.hasTrunk = false;
    m_data.canBlink = false;

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
    char tmpName[DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE + 1];
    for(int i=0; i<DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE; i++) {
        tmpName[i] = m_data.name[i];
    }
    tmpName[DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE] = 0;
    return String(tmpName);
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


bool DynamicProfile::setData(DynamicProfileData data) {
    bool restartRequired = 
        m_data.canBlink != data.canBlink
        || m_data.hasTrunk != data.hasTrunk
        || m_data.hasMovingFrontLights != data.hasMovingFrontLights;

    Serial.println("Setting configuration data:");
    Serial.println("Invert steering = " + String(data.invertSteering));
    Serial.println("Max Positive    = " + String(data.maxAnglePositive));
    Serial.println("Max Negative    = " + String(data.maxAngleNegative));
    Serial.println("Offset          = " + String(data.offsetAngle));
    Serial.println("Name            = " + String(data.name));
    Serial.println("Moving Lights   = " + String(data.hasMovingFrontLights));
    Serial.println("Trunk           = " + String(data.hasTrunk));
    Serial.println("Blink           = " + String(data.canBlink));

    m_data = data;
    save();

    for(const auto& listener : m_listener) {
        listener->onDataChanged();
    }

    return restartRequired;
}

DynamicProfileData DynamicProfile::getData() {
    return m_data;
}

uint16_t DynamicProfile::getDataSize() {
    return sizeof(DynamicProfileData);
}

void DynamicProfile::load() {
    //check if there is any value
    if(EEPROM.read(m_eepromOffset) == 0xFF) {
        Serial.println("found nothing in EEProm, saving defaults");
        save();
    }
    Serial.println("loading data from EEPROM");
    EEPROM.get(m_eepromOffset, m_data);
    Serial.println("Name = " + String(m_data.name));
}

void DynamicProfile::save() {
    Serial.println("saving data to EEPROM");
    Serial.println("Name = " + String(m_data.name));
    EEPROM.put(m_eepromOffset, m_data);
}
