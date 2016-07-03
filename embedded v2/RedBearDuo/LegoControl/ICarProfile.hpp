#ifndef ICARPROFILE_HPP
#define ICARPROFILE_HPP

#include <vector>
#include "Arduino.h"

enum class SteeringDirection {
    Mode1,
    Mode2
};

enum class SupportedCarService {
    MovingFrontLight,
    Trunk,
    Blink
};

class ICarProfile {

public:
    virtual ~ICarProfile() {}

    virtual bool invertSteering() = 0;
    virtual int8_t getMaxSteeringPercentPositive() = 0;
    virtual int8_t getMaxSteeringPercentNegative() = 0;

    virtual std::vector<SupportedCarService> getSupportedCarServices() = 0;

    virtual String getDeviceName() = 0;
};

#endif
