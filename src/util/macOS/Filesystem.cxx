#include <Filesystem.hxx>
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

std::vector<std::string> fs::getDirectoryListing(const std::string &directory)
{
  LOG(LOG_INFO) << "std::vector<std::string> fs::getDirectoryListing not implemented";
  return std::vector<std::string>();
}

std::vector<std::string> fs::getSaveGamePaths()
{
  std::vector<std::string> saveGames;
  LOG(LOG_INFO) << "std::vector<std::string> fs::getSaveGamePaths not implemented";

  return saveGames;
}

bool fs::fileExists(const std::string &filePath) { return true; }

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
