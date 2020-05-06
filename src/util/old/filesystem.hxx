#ifndef FILESYSTEM_HXX_
#define FILESYSTEM_HXX_

#include <ciso646>

#ifndef __ANDROID
#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#else
#pragma message("Warning: Your compiler does not support the filesystem library. Please report this incident to the Cytopia Team")
#endif
#endif

namespace fs
{
#ifndef __ANDROID__
#if __has_include(<filesystem>)
using namespace std::filesystem;
#elif __has_include(<experimental/filesystem>)
using namespace std::experimental::filesystem;
#endif
#endif

/** Read contents from a file as string.
   * @oaram fileName Name of the file to read
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   * @returns the content of the file as string
   */
std::string readFileAsString(const std::string &fileName, bool binaryMode = false);

/** Write a string to a file.
   * @oaram fileName Name of the file to write
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   */
void writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode = false);

/** Get a directory listing of a given directory.
   * @oaram directory Name of the folder to retrieve the directory listing from
   * @returns a directory::iterator containing the folder content
   */
directory_iterator getDirectoryListing(const std::string &directory);

/** Get all savegames in the savegame directory
   * @returns a std::vector containing paths with all savegames
   */
std::vector<path> getSaveGamePaths();

/** Check if a file (or folder) exists
 * @param filePath Path to file or folder to check
 * @returns a true if the path exists
 */

bool fileExists(const std::string &filePath);

/** Get Base Path (where cytopia is being run)
 * A wrapper for SDL_GetBasePath, which is not run on Android because it hard-crashes the app. Memory is freed after the call.
 * @returns the Base path
 */
std::string getBasePath();

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

} // namespace fs
#endif
