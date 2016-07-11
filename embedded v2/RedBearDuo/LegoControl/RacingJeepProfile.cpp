#include "RacingJeepProfile.hpp"

RacingJeepProfile::~RacingJeepProfile() {
}

bool RacingJeepProfile::invertSteering() {
    return false;
}

int8_t RacingJeepProfile::getMaxSteeringAnglePositive() {
    return 100;
}

int8_t RacingJeepProfile::getMaxSteeringAngleNegative() {
    return -100;
}

int8_t RacingJeepProfile::getSteeringOffsetAngle() {
    return 0;
}

bool RacingJeepProfile::hasMovingFrontLightsFeature() {
    return true;
}

bool RacingJeepProfile::hasTrunkFeature() {
    return true;
}

bool RacingJeepProfile::hasBlinkFeature() {
    return true;
}

String RacingJeepProfile::getDeviceName() {
    return "RaceJeep";
}