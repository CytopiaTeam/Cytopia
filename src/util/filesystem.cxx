#include <filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

std::string FileSystem::readFileAsString(const std::string &fileName, bool binaryMode)
{
  std::ios::openmode mode;
  if (binaryMode)
    mode = std::ios_base::in | std::ios_base::binary;
  else
  {
    mode = std::ios_base::in;
  }

  std::ifstream stream(SDL_GetBasePath() + fileName, mode);

  if (!stream)
  {
    LOG(LOG_INFO) << "Open file " << fileName;
    throw ConfigurationError(TRACE_INFO "Can't open file " + fileName);
  }

  std::stringstream buffer;
  buffer << stream.rdbuf();
  stream.close();

  return buffer.str();
}

void FileSystem::writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode)
{
  std::ios::openmode mode;
  if (binaryMode)
  {
    mode = std::ios_base::out | std::ios_base::binary;
  }
  else
  {
    mode = std::ios_base::out;
  }

  std::ofstream stream(SDL_GetBasePath() + fileName, mode);

  if (!stream)
  {
    throw ConfigurationError(TRACE_INFO "Could not write to file " + SDL_GetBasePath() + fileName);
  }

  stream << stringToWrite << std::endl;
  stream.close();
}

fs::directory_iterator FileSystem::getDirectoryListing(const std::string &directory)
{
  std::string pathToSaveFiles = SDL_GetBasePath();
  pathToSaveFiles.append(directory);
  return fs::directory_iterator(std::filesystem::path(pathToSaveFiles));
}

std::vector<fs::path> FileSystem::getSaveGamePaths()
{
  std::vector<fs::path> saveGames;
  forEachFileType("resources", ".cts", [&saveGames](const auto &path) { saveGames.emplace_back(path); });
  return saveGames;
}