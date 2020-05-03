#ifndef FILESYSTEM_HXX_
#define FILESYSTEM_HXX_

#include <ciso646>

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#pragma message("Warning: Your compiler does not support the filesystem library. Please report this incident to the Cytopia Team")
#endif

struct FileSystem
{
  /** Read contents from a file as string.
   * @oaram fileName Name of the file to read
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   * @returns the content of the file as string
   */
  static std::string readFileAsString(const std::string &fileName, bool binaryMode = false);

  /** Write a string to a file.
   * @oaram fileName Name of the file to write
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   */
  static void writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode = false);

  /** Get a directory listing of a given directory.
   * @oaram directory Name of the folder to retrieve the directory listing from
   * @returns a directory::iterator containing the folder content
   */
  static fs::directory_iterator getDirectoryListing(const std::string &directory);

  /** Get all savegames in the savegame directory
   * @returns a std::vector containing paths with all savegames
   */
  static std::vector<fs::path> getSaveGamePaths();
};

template <typename Callback> void forEachFileType(fs::path &&path, std::string &&extension, Callback callback)
{
  for (const auto &fp : fs::directory_iterator(path))
  {
    if (static_cast<std::filesystem::path>(fp).extension() == extension)
    {
      callback(fp);
    }
  }
}
#endif
