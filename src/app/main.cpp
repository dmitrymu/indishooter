// Public domain software (CC0).

#include "options.h"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>

#include "FilenameGenerator.h"
#include "ShooterClient.h"

using namespace Shooter;
using namespace std::chrono_literals;

namespace
{   // TODO: move list and shoot functions to library?

void listDevices(Shooter::Client &client)
{
    std::this_thread::sleep_for(.1s);
    auto devices = client.getDevices();
    std::cout << devices.size() << " devices found" << std::endl;
    for (const auto &device : devices) {
        std::cout << device.getDeviceName() << std::endl;
    }
}

/** Format exposure and gain as a part of filename.
 */
std::string formatPostTag(const Options::ShootParams &params)
{
    auto expString = std::format("{:f}", params.exposure);

    // Replace decimal dot with underscore: 1.5 s -> e1_5000
    const auto point = expString.find(".");
    if (point != std::string::npos) {
        expString.replace(point, 1, "_");
    }

    if (point != std::string::npos) {
        // Remove trailing zeroes after decimal point: e1_5000 -> e1_5
        while (expString.length() > 0 && expString.back() == '0') {
            expString.pop_back();
        }

        // Remove trailing underscore: 2 s -> e2_0000 -> e2_ -> e2
        if (expString.length() > 0 && expString.back() == '_') {
            expString.pop_back();
        }
    }

    // Assume gain is an integer.
    return std::format("e{}_g{}", expString, (unsigned)params.gain);
}

/** Shoot a batch of photos with the same gain and exposure.
*/
void shootBatch(Shooter::Client &client,
                FilenameGenerator &fg,
                const Options::ShootParams &params)
{
    const auto blobDelay = 5s;
    const auto exposure = Client::ExposureT{params.exposure};
    client.setGain(params.gain);
    for (unsigned n = 0; n < params.count; ++n) {
        client.startExposure(exposure);
        auto blob = client.getBlob(exposure + blobDelay);
        const auto path = fg.nextFile(formatPostTag(params));
        std::cout << "Saving to " << path << std::endl;
        std::ofstream out(path, std::ios::binary);
        out.write(reinterpret_cast<const char *>(blob.data()), blob.size());
    }
}

/** Shoot batchers in sequence with the specified interval.
*/
void shoot(Shooter::Client &client,
           FilenameGenerator &fg,
           const Options::ShootParams &params)
{
    const auto delay = Client::ExposureT{params.delay};
    auto time = std::chrono::system_clock::now();
    for (unsigned n = 0; n < params.repeat; ++n) {
        fg.nextBatch();
        if (n > 0) {
            std::this_thread::sleep_until(time + delay);
            time = std::chrono::system_clock::now();
        }
        shootBatch(client, fg, params);
    }
}

} // namespace

int main(int argc, const char **argv)
{
    try {
        Options options;

        options.Parse(argc, argv);

        auto client = std::make_unique<Shooter::Client>();
        // client->setVerbose(true);
        client->setServer(options.indiHost.c_str(), options.indiPort);

        if (!client->connectServer()) {
            std::cerr << "Failed to connect to " << options.indiHost << ":"
                      << options.indiPort << std::endl;
            exit(EXIT_FAILURE);
        }

        if (options.listDevices) {
            listDevices(*client);
        } else {
            client->initCamera(options.cameraName);
            std::this_thread::sleep_for(.5s);
            FilenameGenerator fg(options.dir, options.tag);
            shoot(*client, fg, options.shootParams);
        }

        client->disconnectServer();
    } catch (const std::exception &ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
