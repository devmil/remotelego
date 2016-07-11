#ifndef RACINGJEEPPROFILE_HPP
#define RACINGJEEPPROFILE_HPP

#include "ICarProfile.hpp"

class RacingJeepProfile : public virtual ICarProfile {

    ~RacingJeepProfile() override;

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
