#ifndef DUMPERPROFILE_HPP
#define DUMPERPROFILE_HPP

#include "ICarProfile.hpp"

class DumperProfile : public virtual ICarProfile {

    ~DumperProfile() override {}

    SteeringDirection getSteeringDirection() override {
        return SteeringDirection::Mode2;
    }

    int8_t getMaxSteeringPercentPositive() override {
        return 40;
    }

    int8_t getMaxSteeringPercentNegative() override {
        return -70;
    }

    std::vector<SupportedCarService> getSupportedCarServices() override {
        return std::vector<SupportedCarService> {
          SupportedCarService::Trunk
        };
    }

    String getDeviceName() override {
      return "Dumper";
    }

};

#endif
