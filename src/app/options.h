#ifndef _INDISHOOTER_OPTIONS_H
#define _INDISHOOTER_OPTIONS_H

#pragma once

#include <string>
#include <cstdint>

namespace Shooter
{
struct Options
{
    bool printHelp {false};
    bool listDevices {false};

    struct ShootParams
    {
        double exposure{1.0};
        double gain{0};
        double offset{0};
        unsigned count{1};
        double delay{0};
        unsigned repeat{1}; 
    } shootParams;

    std::string dir;
    std::string tag;

    uint16_t indiPort{7624};
    std::string indiHost{"localhost"};
    std::string cameraName{};

    // Parse option, exit on '--help'
    void Parse(int argc, const char** argv);
};
}

#endif
