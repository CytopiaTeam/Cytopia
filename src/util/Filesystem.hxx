#ifndef FILESYSTEM_HXX_
#define FILESYSTEM_HXX_

#include <ciso646>
#include <string>
#include <vector>

namespace fs
{

/** Read contents from a file as string.
   * @param fileName Name of the file to read
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
std::vector<std::string> getDirectoryListing(const std::string &directory);

/** Get all savegames in the savegame directory
   * @returns a std::vector containing paths with all savegames
   */
std::vector<std::string> getSaveGamePaths();

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

} // namespace fs
#endif
