#ifndef RACINGJEEPPROFILE_HPP
#define RACINGJEEPPROFILE_HPP

#include "ICarProfile.hpp"

class RacingJeepProfile : public virtual ICarProfile {

    ~RacingJeepProfile() override;

    bool invertSteering() override;

    int8_t getMaxSteeringPercentPositive() override;

    int8_t getMaxSteeringPercentNegative() override;

    std::vector<SupportedCarService> getSupportedCarServices() override;

    String getDeviceName() override;
};

#endif
