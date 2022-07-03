#ifndef FILESYSTEM_HXX_
#define FILESYSTEM_HXX_

#include <ciso646>
#include <string>
#include <vector>

namespace fs
{

/** @brief Read contents from a file as string.
   * @param fileName Name of the file to read
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   * @returns the content of the file as string
   */
std::string readFileAsString(const std::string &fileName, bool binaryMode = false);

/** @brief Read contents from a file as string.
   * @param fileName Name of the file to read
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   * @returns the content of the file as string
   */
std::string readCompressedFileAsString(const std::string &fileName);

/** @brief Write a string to a file.
   * @param fileName Name of the file to write
   * @param stringToWrite string to be written
   * @param binaryMode (optional) open the file in binary mode. (default: false)
   */
void writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode = false);

/** @brief Write a string to a file and compress it.
   * @param fileName Name of the file to write
   * @param stringToWrite string to be written
   */
void writeStringToFileCompressed(const std::string &fileName, const std::string &stringToWrite);

/** @brief Get a directory listing of a given directory.
   * @param directory Name of the folder to retrieve the directory listing from
   * @returns a directory::iterator containing the folder content
   */
std::vector<std::string> getDirectoryListing(const std::string &directory);

/** @brief Get all savegames in the savegame directory
   * @returns a std::vector containing paths with all savegames
   */
std::vector<std::string> getSaveGamePaths();

/** @brief Check if a file (or folder) exists
 * @param filePath Path to file or folder to check
 * @returns true if the path exists
 */

bool fileExists(const std::string &filePath);

/** @brief Get base path (where Cytopia is being run)
 * @details A wrapper for SDL_GetBasePath, which is not run on Android because it hard-crashes the app. Memory is freed after the call.
 * @returns the Base path
 */
std::string getBasePath();

void createDirectory(const std::string &dir);

} // namespace fs
#endif
