#include "options.h"

#include <boost/program_options.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>

using namespace Shooter;
namespace po = boost::program_options;

namespace 
{
    const char VERSION_MESSAGE[] = "indishooter version " PROJECT_VERISON; 
}

void Options::Parse(int argc, const char **argv)
{
    // clang-format off
    po::options_description general("General");
    general.add_options()
        ("help,h", "print help message")
        ("version,v", "print program version")
        ("list,l", po::bool_switch(&listDevices), "list devices instead of shooting");

    po::options_description indi("INDI service");
    indi.add_options()
        ("host,H", po::value<std::string>(&indiHost)->default_value("localhost"), "INDI host")
        ("port,P", po::value<uint16_t>(&indiPort)->default_value(7624), "INDI port");

    po::options_description camera("CCD/Camera");
    camera.add_options()
        ("name,n", po::value<std::string>(&cameraName), "camera name")
        ("exposure,x", po::value<double>(&shootParams.exposure)->default_value(1.0), "exposure in seconds")
        ("gain,g", po::value<double>(&shootParams.gain)->default_value(0), "gain")
        ("offset,o", po::value<double>(&shootParams.offset)->default_value(10), "offset");

    po::options_description batch("Batching");
    batch.add_options()
        ("batch,b", po::value<unsigned>(&shootParams.count)->default_value(1), "number of frames in the batch")
        ("interval,i", po::value<double>(&shootParams.delay)->default_value(0.0), "interval between batches in seconds")
        ("count,c", po::value<unsigned>(&shootParams.repeat)->default_value(1), "number of batches");

    po::options_description file("File saving");
    file.add_options()
        ("dir,d", po::value<std::string>(&dir)->default_value("light"), "output directory for FITS files")
        ("tag,t", po::value<std::string>(&tag)->default_value("frame"),"tag to start each file name");

    // Declare the supported options.
    po::options_description all(
        "Utility to shoot astrophotos in batches using INDI service\n"
        "Available options:");
    all.add(general).add(indi).add(camera).add(batch).add(file);
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all), vm);
    po::notify(vm);

    if (vm.count("version")) {
        std::cerr << VERSION_MESSAGE << std::endl;
        exit(EXIT_SUCCESS);
    } else if (vm.count("help")) {
        std::cerr << VERSION_MESSAGE << '\n' << all << std::endl;
        exit(EXIT_SUCCESS);
    }
}
