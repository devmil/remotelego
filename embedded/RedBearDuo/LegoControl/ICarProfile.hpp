#ifndef ICARPROFILE_HPP
#define ICARPROFILE_HPP

#include <vector>
#include "Arduino.h"

class ICarProfile {

public:
    virtual ~ICarProfile() {}

    virtual bool invertSteering() = 0;
    virtual int8_t getMaxSteeringAnglePositive() = 0;
    virtual int8_t getMaxSteeringAngleNegative() = 0;
    virtual int8_t getSteeringOffsetAngle() = 0;

    virtual bool hasMovingFrontLightsFeature() = 0;
    virtual bool hasTrunkFeature() = 0;
    virtual bool hasBlinkFeature() = 0;

    virtual String getDeviceName() = 0;
};

#endif
