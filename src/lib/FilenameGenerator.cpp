// Public domain software (CC0).

#include "FilenameGenerator.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace Shooter;
namespace fs = std::filesystem;

FilenameGenerator::FilenameGenerator(const std::string &dir,
                                     const std::string &tag)
    : dir{dir}, tag{tag}
{
    if (!fs::exists(dir)) {
        std::cout << "Creating " << dir << std::endl;
        fs::create_directories(dir);
    }

    if (!fs::is_empty(dir)) {
        throw std::runtime_error(std::format("Directory {} is not empty", dir));
    }
}

void FilenameGenerator::nextBatch()
{
    batch++;
    frame = 0;
}

std::string FilenameGenerator::nextFile(const std::string &postTag /*= ""*/)
{
    ++frame;
    const auto fileName =
        std::format("{}_{}_{:03}_{:03}.{}", tag, postTag, batch, frame, ext);
    const auto result = fs::path(dir) / fileName;
    if (fs::exists(result)) {
        throw std::runtime_error(
            std::format("File {} already exists", result.string()));
    }
    return result;
}
