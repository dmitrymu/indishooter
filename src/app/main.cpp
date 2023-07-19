#include "options.h"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <fstream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>

#include "ShooterClient.h"
#include "FilenameGenerator.h"

using namespace Shooter;
using namespace std::chrono_literals;

void listDevices(Shooter::Client& client)
{
    std::this_thread::sleep_for(.1s);
    auto devices = client.getDevices();
    std::cout << devices.size() << " devices found" << std::endl;
    for (const auto &device : devices)
    {
        std::cout << device.getDeviceName()  << std::endl;
    }
}

std::string formatPostTag(const Options::ShootParams& params)
{
    auto expString = std::format("{:f}", params.exposure);
    const auto point = expString.find(".");
    if (point != std::string::npos)
    {
        expString.replace(point, 1, "_");
    } 
    if (point)
    {
        while(expString.length() > 0 && expString.back() == '0')
        {
            expString.pop_back();
        }
    }

    return std::format("e{}_g{}", expString, (unsigned)params.gain);
}

void shootBatch(Shooter::Client& client, FilenameGenerator& fg, const Options::ShootParams& params)
{
    const auto blobDelay = 5s;
    const auto exposure = Client::ExposureT{params.exposure};
    client.setGain(params.gain);
    for (unsigned n = 0; n < params.count; ++n)
    {
        client.startExposure(exposure);
        auto blob = client.getBlob(exposure + blobDelay);
        const auto path = fg.nextFile(formatPostTag(params));
        std::cout << "Saving to " << path <<std::endl;
        std::ofstream out(path, std::ios::binary);
        out.write(reinterpret_cast<const char*>(blob.data()), blob.size());
    }
}

void shoot(Shooter::Client& client, FilenameGenerator& fg, const Options::ShootParams& params)
{
    const auto delay = Client::ExposureT{params.delay};
    auto time = std::chrono::system_clock::now();
    for (unsigned n = 0; n < params.repeat; ++n)
    {
        fg.nextBatch();
        if (n > 0)
        {
            std::this_thread::sleep_until(time + delay);
            time = std::chrono::system_clock::now();
        }
        shootBatch(client, fg, params);
    }
}

int main(int argc, const char** argv)
{
    try {
        Options options;

        options.Parse(argc, argv);

        auto client = std::make_unique<Shooter::Client>();
        //client->setVerbose(true);
        client->setServer(options.indiHost.c_str(), options.indiPort);
        
        if(!client->connectServer()) {
            std::cerr << "Failed to connect to "
            << options.indiHost << ":" << options.indiPort << std::endl;
            exit(EXIT_FAILURE);
        }

        if (options.listDevices)
        {
            listDevices(*client);
        } else {
            client->initCamera(options.cameraName);
            std::this_thread::sleep_for(.5s);
            FilenameGenerator fg(options.dir, options.tag);
            shoot(*client, fg, options.shootParams);
        }

        client->disconnectServer(); 
    } catch (const std::exception& ex ) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
