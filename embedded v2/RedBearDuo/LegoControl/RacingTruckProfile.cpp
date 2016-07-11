#include "RacingTruckProfile.hpp"

RacingTruckProfile::~RacingTruckProfile() {
}

bool RacingTruckProfile::invertSteering() {
    return true;
}

int8_t RacingTruckProfile::getMaxSteeringAnglePositive() {
    return 100;
}

int8_t RacingTruckProfile::getMaxSteeringAngleNegative() {
    return -100;
}

int8_t RacingTruckProfile::getSteeringOffsetAngle() {
    return 40;
}

bool RacingTruckProfile::hasMovingFrontLightsFeature() {
    return false;
}

bool RacingTruckProfile::hasTrunkFeature() {
    return false;
}

bool RacingTruckProfile::hasBlinkFeature() {
    return true;
}

String RacingTruckProfile::getDeviceName() {
    return "R-Truck";
}
