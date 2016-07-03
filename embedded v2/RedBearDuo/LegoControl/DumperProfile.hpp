#ifndef DUMPERPROFILE_HPP
#define DUMPERPROFILE_HPP

#include "ICarProfile.hpp"

class DumperProfile : public virtual ICarProfile {

    ~DumperProfile() override;

    bool invertSteering() override;

    int8_t getMaxSteeringPercentPositive() override;

    int8_t getMaxSteeringPercentNegative() override;

    std::vector<SupportedCarService> getSupportedCarServices() override;

    String getDeviceName() override;
};

#endif
