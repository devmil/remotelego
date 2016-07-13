#include "RacingTruckProfile.hpp"

RacingTruckProfile::~RacingTruckProfile() {
}

bool RacingTruckProfile::invertSteering() {
    return true;
}

int8_t RacingTruckProfile::getMaxSteeringAnglePositive() {
    return 90;
}

int8_t RacingTruckProfile::getMaxSteeringAngleNegative() {
    return -90;
}

int8_t RacingTruckProfile::getSteeringOffsetAngle() {
    return 10;
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
