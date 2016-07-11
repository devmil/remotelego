#include "DumperProfile.hpp"

DumperProfile::~DumperProfile() {
}

bool DumperProfile::invertSteering() {
    return true;
}

int8_t DumperProfile::getMaxSteeringAnglePositive() {
    return 40;
}

int8_t DumperProfile::getMaxSteeringAngleNegative() {
    return -70;
}

int8_t DumperProfile::getSteeringOffsetAngle() {
    return 0;
}

bool DumperProfile::hasMovingFrontLightsFeature() {
    return false;
}

bool DumperProfile::hasTrunkFeature() {
    return false;
}

bool DumperProfile::hasBlinkFeature() {
    return true;
}

String DumperProfile::getDeviceName() {
    return "Dumper";
}