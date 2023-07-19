#include "options.h"

#include <boost/program_options.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>

using namespace Shooter;
namespace po = boost::program_options;

void Options::Parse(int argc, const char **argv)
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "print help message")
        ("list,l", po::bool_switch(&listDevices), "list devices instead of shooting")
        ("host,H", po::value<std::string>(&indiHost)->default_value("localhost"), "INDI host")
        ("port,P", po::value<uint16_t>(&indiPort)->default_value(7624), "INDI port")
        ("name,n", po::value<std::string>(&cameraName), "camera name")
        ("exposure,x", po::value<double>(&shootParams.exposure)->default_value(1.0), "exposure in seconds")
        ("gain,g", po::value<double>(&shootParams.gain)->default_value(0), "gain")
        ("offset,o", po::value<double>(&shootParams.offset)->default_value(10), "offset")
        ("batch,b", po::value<unsigned>(&shootParams.count)->default_value(1), "number of frames in the batch")
        ("interval,i", po::value<double>(&shootParams.delay)->default_value(0.0), "interval between batches in seconds")
        ("count,c", po::value<unsigned>(&shootParams.repeat)->default_value(1), "number of batches")
        ("dir,d", po::value<std::string>(&dir)->default_value("light"), "output directory")
        ("tag,t", po::value<std::string>(&tag)->default_value("frame"),"tag to start each file name");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cerr << desc << std::endl;
        exit(EXIT_FAILURE);
    }
}
