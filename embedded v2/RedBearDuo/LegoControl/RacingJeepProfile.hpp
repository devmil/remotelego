#ifndef RACINGJEEPPROFILE_HPP
#define RACINGJEEPPROFILE_HPP

#include "ICarProfile.hpp"

class RacingJeepProfile : public virtual ICarProfile {

    ~RacingJeepProfile() override {}

    SteeringDirection getSteeringDirection() override {
        return SteeringDirection::Mode1;
    }

    int8_t getMaxSteeringPercentPositive() override {
        return 100;
    }

    int8_t getMaxSteeringPercentNegative() override {
        return -100;
    }

    std::vector<SupportedCarService> getSupportedCarServices() override {
        return std::vector<SupportedCarService> { 
            SupportedCarService::MovingFrontLight, 
            SupportedCarService::Trunk,
            SupportedCarService::Blink
        };
    }

    String getDeviceName() override {
      return "RaceJeep";
    }
};

#endif
