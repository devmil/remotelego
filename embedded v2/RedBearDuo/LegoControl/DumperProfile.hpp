#ifndef DUMPERPROFILE_HPP
#define DUMPERPROFILE_HPP

#include "ICarProfile.hpp"

class DumperProfile : public virtual ICarProfile {

    ~DumperProfile() override;

    bool invertSteering() override;

    int8_t getMaxSteeringAnglePositive() override;

    int8_t getMaxSteeringAngleNegative() override;

    int8_t getSteeringOffsetAngle() override;

    bool hasMovingFrontLightsFeature() override;
    
    bool hasTrunkFeature() override;
    
    bool hasBlinkFeature() override;

    String getDeviceName() override;
};

#endif
