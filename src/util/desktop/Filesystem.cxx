#include <filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

std::string fs::readFileAsString(const std::string &fileName, bool binaryMode)
{
  std::ios::openmode mode;
  if (binaryMode)
    mode = std::ios_base::in | std::ios_base::binary;
  else
  {
    mode = std::ios_base::in;
  }

  if (!fileExists(getBasePath() + fileName))
  {
    throw ConfigurationError(TRACE_INFO "File doesn't exist:" + fileName);
  }

  std::ifstream stream(getBasePath() + fileName, mode);

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

void fs::writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode)
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

  std::ofstream stream(getBasePath() + fileName, mode);

  if (!stream)
  {
    throw ConfigurationError(TRACE_INFO "Could not write to file " + getBasePath() + fileName);
  }

  stream << stringToWrite << std::endl;
  stream.close();
}

fs::directory_iterator fs::getDirectoryListing(const std::string &directory)
{
  std::string pathToSaveFiles = getBasePath();
  pathToSaveFiles.append(directory);
  return directory_iterator(std::filesystem::path(pathToSaveFiles));
}

std::vector<fs::path> fs::getSaveGamePaths()
{
  std::vector<path> saveGames;
  forEachFileType("resources", ".cts", [&saveGames](const auto &path) { saveGames.emplace_back(path); });
  return saveGames;
}

bool fs::fileExists(const std::string &filePath) { return fs::exists(fs::path(filePath)); }

std::string fs::getBasePath()
{
  std::string sPath;

  char *path = SDL_GetBasePath();
  if (path)
  {
    sPath = {path};
  }
  else
  {
    throw CytopiaError(TRACE_INFO "SDL_GetBasePath() failed!");
  }
  SDL_free(path);

  return sPath;
}
