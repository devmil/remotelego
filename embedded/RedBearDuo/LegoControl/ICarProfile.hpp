#ifndef ICARPROFILE_HPP
#define ICARPROFILE_HPP

#include <vector>
#include "Arduino.h"

class ICarProfileDataChangedListener {
public:
    virtual ~ICarProfileDataChangedListener() {}
    
    virtual void onDataChanged() = 0;
};

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

    virtual void addListener(ICarProfileDataChangedListener* listener) = 0;
    virtual void removeListener(ICarProfileDataChangedListener* listener) = 0;
};

#endif
