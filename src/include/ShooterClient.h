#pragma once

#include "TEvent.h"
#include "libindi/baseclient.h"
#include "libindi/basedevice.h"

#include <chrono>
#include <ratio>
#include <string>

namespace Shooter
{
/**  INDI client providing CCD shooting interface.
 */
class Client : public INDI::BaseClient
{
  public:
    /// FITS blob descriptor.
    using BlobT = std::span<const uint8_t>;

    /// chrono-compatible time interval.
    using ExposureT = std::chrono::duration<double>;

    /// Initialize underlying libindi client.
    Client();

    /** Set up a camera connection.
        Waits until the camera is initialized.
    */
    void initCamera(const std::string &name);

    /// Send the new gain value to the camera driver.
    void setGain(double gain);

    /** Send the exposure value to the camera driver.
        It triggers frame acquisition.
        getBlob() must be called before next startExposure().
    */
    void startExposure(ExposureT exp);

    /** Wait until the camera driver takes the frame and return it.
    */
    BlobT getBlob(ExposureT delay);

  private:
    void onNewDevice(INDI::BaseDevice device);
    void watchConnection(INDI::Property property);
    void watchBlob(INDI::PropertyBlob property);
    void log(const std::string &message) const;

    std::string name;
    INDI::BaseDevice ccd;

    SimpleEvent connectEvent;
    TEvent<BlobT> blobEvent;
};
} // namespace Shooter