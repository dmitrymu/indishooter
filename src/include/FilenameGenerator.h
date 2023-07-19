#pragma once
#include <string>
namespace Shooter 
{
class FilenameGenerator
{
  public:
    FilenameGenerator(const std::string &dir, const std::string &tag);

    void nextBatch();
    std::string nextFile(const std::string& postTag = "");


  private:
    std::string dir;
    std::string tag;
    std::string ext{"fits"};
    unsigned batch{0};
    unsigned frame{0};
};
}