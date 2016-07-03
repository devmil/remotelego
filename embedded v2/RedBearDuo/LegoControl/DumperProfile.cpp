#include "DumperProfile.hpp"

DumperProfile::~DumperProfile() {
}

bool DumperProfile::invertSteering() {
    return true;
}

int8_t DumperProfile::getMaxSteeringPercentPositive() {
    return 40;
}

int8_t DumperProfile::getMaxSteeringPercentNegative() {
    return -70;
}

std::vector<SupportedCarService> DumperProfile::getSupportedCarServices() {
    return std::vector<SupportedCarService> {
        SupportedCarService::Trunk
    };
}

String DumperProfile::getDeviceName() {
    return "Dumper";
}