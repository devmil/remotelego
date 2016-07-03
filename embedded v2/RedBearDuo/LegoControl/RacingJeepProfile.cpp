#include "RacingJeepProfile.hpp"

RacingJeepProfile::~RacingJeepProfile() {
}

bool RacingJeepProfile::invertSteering() {
    return false;
}

int8_t RacingJeepProfile::getMaxSteeringPercentPositive() {
    return 100;
}

int8_t RacingJeepProfile::getMaxSteeringPercentNegative() {
    return -100;
}

std::vector<SupportedCarService> RacingJeepProfile::getSupportedCarServices() {
    return std::vector<SupportedCarService> { 
        SupportedCarService::MovingFrontLight, 
        SupportedCarService::Trunk,
        SupportedCarService::Blink
    };
}

String RacingJeepProfile::getDeviceName() {
    return "RaceJeep";
}