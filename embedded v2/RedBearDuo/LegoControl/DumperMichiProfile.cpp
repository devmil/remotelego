#include "DumperMichiProfile.hpp"

DumperMichiProfile::~DumperMichiProfile() {
}

bool DumperMichiProfile::invertSteering() {
    return true;
}

int8_t DumperMichiProfile::getMaxSteeringAnglePositive() {
    return 70;
}

int8_t DumperMichiProfile::getMaxSteeringAngleNegative() {
    return -70;
}

int8_t DumperMichiProfile::getSteeringOffsetAngle() {
    return 0;
}

bool DumperMichiProfile::hasMovingFrontLightsFeature() {
    return false;
}

bool DumperMichiProfile::hasTrunkFeature() {
    return false;
}

bool DumperMichiProfile::hasBlinkFeature() {
    return true;
}

String DumperMichiProfile::getDeviceName() {
    return "Dumper-Michi";
}