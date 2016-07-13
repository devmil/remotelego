#include "RacingJeepProfile.hpp"

RacingJeepProfile::~RacingJeepProfile() {
}

bool RacingJeepProfile::invertSteering() {
    return false;
}

int8_t RacingJeepProfile::getMaxSteeringAnglePositive() {
    return 90;
}

int8_t RacingJeepProfile::getMaxSteeringAngleNegative() {
    return -90;
}

int8_t RacingJeepProfile::getSteeringOffsetAngle() {
    return 10;
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
