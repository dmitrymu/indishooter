#pragma once
  
#include "libindi/baseclient.h"
#include "libindi/basedevice.h"
#include "TEvent.h"

#include <chrono>
#include <ratio>
#include <string>

namespace Shooter
{
class Client : public INDI::BaseClient
{
  public:
    using BlobT = std::span<const uint8_t>;
    using ExposureT = std::chrono::duration<double>;

    Client();

    void initCamera(const std::string& name);

    void setGain(double gain);

    void startExposure(ExposureT exp);

    BlobT getBlob(ExposureT delay);

  private:

    void onNewDevice(INDI::BaseDevice device);
    void watchConnection(INDI::Property property);
    void watchBlob(INDI::PropertyBlob property);
    void log(const std::string& message) const;

    std::string name;
    INDI::BaseDevice ccd;

    SimpleEvent connectEvent;
    TEvent<BlobT> blobEvent;
};
} // namespace Shooter