#ifndef DYNAMICPROFILE_HPP
#define DYNAMICPROFILE_HPP

#include "ICarProfile.hpp"

#define DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE 20

struct DynamicProfileData {
    bool invertSteering;
    int8_t maxAnglePositive;
    int8_t maxAngleNegative;
    int8_t offsetAngle;

    char name[DYNAMIC_PROFILE_DATA_MAX_NAME_SIZE];

    bool hasMovingFrontLights;
    bool hasTrunk;
    bool canBlink;
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

/**
*  Dynamic profile members
*/
    /**
    *   \brief returns [true] when a restart is required, otherwise [false]   
    */
    bool setData(DynamicProfileData data);

    DynamicProfileData getData();

    uint16_t getDataSize();
private:
    DynamicProfileData m_data;
    int m_eepromOffset;

    void load();
    void save();
};

#endif