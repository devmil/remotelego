#include "DumperFloProfile.hpp"

DumperFloProfile::~DumperFloProfile() {
}

bool DumperFloProfile::invertSteering() {
    return true;
}

int8_t DumperFloProfile::getMaxSteeringAnglePositive() {
    return 40;
}

int8_t DumperFloProfile::getMaxSteeringAngleNegative() {
    return -70;
}

int8_t DumperFloProfile::getSteeringOffsetAngle() {
    return 0;
}

bool DumperFloProfile::hasMovingFrontLightsFeature() {
    return false;
}

bool DumperFloProfile::hasTrunkFeature() {
    return false;
}

bool DumperFloProfile::hasBlinkFeature() {
    return true;
}

String DumperFloProfile::getDeviceName() {
    return "Dumper-Flo";
}