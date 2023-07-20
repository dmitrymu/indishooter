// Public domain software (CC0).

#pragma once

#include <cstdint>
#include <string>

namespace Shooter
{
/** Program options
*/
struct Options {
    // INDI server host.
    std::string indiHost{"localhost"};

    // INDI server port.
    uint16_t indiPort{7624};

    /// List INDI devices connected to the server instead of shooting.
    bool listDevices{false};

    // CCD name.
    std::string cameraName{};

    // All shooting parameters.
    struct ShootParams {

        // Exposure in seconds.
        double exposure{1.0};

        // CCD gain.
        double gain{0};

        // CCD offset.
        double offset{0};

        // Number of frames in the batch.
        unsigned count{1};

        // Delay between batches.
        double delay{0};

        // Number of batches in sequence.
        unsigned repeat{1};
    } shootParams;

    // Directory to store FITS files.
    std::string dir;

    // Tag to stat filename with.
    std::string tag;

    // Parse option, exit on '--help' and '--version'.
    void Parse(int argc, const char **argv);
};
} // namespace Shooter
