// Public domain software (CC0).

#pragma once
#include <string>
namespace Shooter
{
/** Generate unique file names.

    File name pattern is 
    {tag}_{post_tag}_{batch}_{frame}.fits
*/  
class FilenameGenerator
{
  public:
    /** Initialize batch and frame counts, create directory if not exist.

        Throws if @dir already exists and is not empty.
    */
    FilenameGenerator(const std::string &dir, const std::string &tag);

    /// Advance batch counter
    void nextBatch();

    /// Advance frame counter and return full path to the next file.
    std::string nextFile(const std::string &postTag = "");

  private:
    std::string dir;
    std::string tag;
    std::string ext{"fits"};
    unsigned batch{0};
    unsigned frame{0};
};
} // namespace Shooter