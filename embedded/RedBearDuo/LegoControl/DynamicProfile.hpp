#ifndef DYNAMICPROFILE_HPP
#define DYNAMICPROFILE_HPP

#include <vector>

#include "ICarProfile.hpp"

#define DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE 20 //so that the total amount of data is 20 (seems to be a limitation in the BLE stack)

struct DynamicProfileData {
    bool invertSteering;
    int8_t maxAnglePositive;
    int8_t maxAngleNegative;
    int8_t offsetAngle;

    bool hasMovingFrontLights;
    bool hasTrunk;
    bool canBlink;
};

struct DynamicProfileNameData {
    uint8_t name[DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE];
};


class DynamicProfile : public virtual ICarProfile {
public:

    DynamicProfile(int eepromOffset);
    virtual ~DynamicProfile();

/**
*  ICarProfile members
*/
    bool invertSteering() override;

    int8_t getMaxSteeringAnglePositive() override;

    int8_t getMaxSteeringAngleNegative() override;

    int8_t getSteeringOffsetAngle() override;

    bool hasMovingFrontLightsFeature() override;
    
    bool hasTrunkFeature() override;
    
    bool hasBlinkFeature() override;

    String getDeviceName() override;

    void addListener(ICarProfileDataChangedListener* listener) override;
    void removeListener(ICarProfileDataChangedListener* listener) override;


/**
*  Dynamic profile members
*/
    void setData(DynamicProfileData data);

    DynamicProfileData getData();

    uint16_t getDataSize();

    uint16_t getNameDataSize();
    DynamicProfileNameData getNameData();
    void setNameData(DynamicProfileNameData nameData);
private:
    DynamicProfileData m_data;
    DynamicProfileNameData m_nameData;
    int m_eepromOffset;
    std::vector<ICarProfileDataChangedListener*> m_listener;

    void load();
    void save();

    void printDataToLog();

    void setNameFromString(String name);
    String getNameAsString();
};

#endif