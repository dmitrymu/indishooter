// Public domain software (CC0).

#include "ShooterClient.h"
#include "basedevice.h"
#include "indiccd.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

using namespace Shooter;
using namespace std::chrono_literals;

namespace
{
constexpr std::string PROPERTY_CONNECTION{"CONNECTION"};
constexpr std::string PROPERTY_BLOB{"CCD1"};

std::string to_string(std::thread::id id)
{
    std::ostringstream s;
    s << std::hex << id;
    return s.str();
}
} // namespace

Client::Client() {}

void Client::initCamera(const std::string &cameraName)
{
    using namespace std::placeholders;

    name = cameraName;
    if (name.empty()) {
        log("Camera was not specified");
    }
    setBLOBMode(B_ALSO, name.c_str(), nullptr);
    enableDirectBlobAccess(name.c_str(), nullptr);
    auto device = getDevice(name.c_str());
    if (device.isValid()) {
        onNewDevice(device);
    } else {
        watchDevice(name.c_str(), std::bind(&Client::onNewDevice, this, _1));
    }

    connectEvent.wait_for(1s);
}

void Client::setGain(double gain)
{
    INDI::PropertyNumber propGain = ccd.getProperty("CCD_GAIN");

    if (!propGain.isValid()) {
        log(std::format("Error: unable to find {} CCD_GAIN property...", name));
        return;
    }

    propGain[0].setValue(gain);
    sendNewProperty(propGain);
}

void Client::startExposure(ExposureT exp)
{
    INDI::PropertyNumber ccdExposure = ccd.getProperty("CCD_EXPOSURE");

    if (!ccdExposure.isValid()) {
        log("Error: unable to find CCD Simulator CCD_EXPOSURE property...");
        return;
    }

    // Take a 1 second exposure
    log(std::format("Taking {} second exposure...", exp.count()));
    ccdExposure[0].setValue(exp.count());
    sendNewProperty(ccdExposure);
}

Client::BlobT Client::getBlob(ExposureT delay)
{
    return blobEvent.wait_for(delay);
}

void Client::onNewDevice(INDI::BaseDevice device)
{
    using namespace std::placeholders;
    if (device.getDeviceName() == name) {
        log(std::format("Connecting to {}", name));
        ccd = device;
        device.watchProperty(PROPERTY_CONNECTION.c_str(),
                             std::bind(&Client::watchConnection, this, _1),
                             INDI::BaseDevice::WATCH_NEW);
        device.watchProperty(PROPERTY_BLOB.c_str(),
                             std::bind(&Client::watchBlob, this, _1),
                             INDI::BaseDevice::WATCH_UPDATE);
    } else {
        log(std::format("Unexpected device name {}", device.getDeviceName()));
    }
}

void Client::watchConnection(INDI::Property property)
{
    if (property.getName() == PROPERTY_CONNECTION) {
        log(std::format("Connecting to INDI device {} ...", name));
        connectDevice(name.c_str());
        connectEvent.raise();
    } else {
        log(std::format("Unexpected property name {}", property.getName()));
    }
}

void Client::watchBlob(INDI::PropertyBlob property)
{
    const BlobT b{static_cast<const uint8_t *>(property[0].getBlob()),
                  static_cast<size_t>(property[0].getBlobLen())};
    log(std::format("Received blob of {} bytes", b.size()));
    blobEvent.raise(b);
}

void Client::log(const std::string &message) const
{
    std::thread::id a;
    std::cout << std::format("[{} {:%X}] {}",
                             to_string(std::this_thread::get_id()),
                             std::chrono::system_clock::now(), message)
              << std::endl;
}
