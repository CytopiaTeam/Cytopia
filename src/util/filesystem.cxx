#include <filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

std::string FileSystem::readFileAsString(const std::string &fileName)
{
  std::ifstream stream(SDL_GetBasePath() + fileName);

  if (!stream)
  {
    throw ConfigurationError(TRACE_INFO "Can't open file " + fileName);
  }

  std::stringstream buffer;
  buffer << stream.rdbuf();
  stream.close();

  return buffer.str();
}

std::string FileSystem::readFileAsBinary(const std::string &fileName)
{
  std::ifstream stream(SDL_GetBasePath() + fileName, std::ios_base::in | std::ios_base::binary);

  if (!stream)
  {
    throw ConfigurationError(TRACE_INFO "Can't open file " + fileName);
  }

  std::stringstream buffer;
  buffer << stream.rdbuf();
  stream.close();

  return buffer.str();
}

void FileSystem::writeStringToFile(const std::string &fileName, const std::string &stringToWrite)
{
  // not implemented yet
}
