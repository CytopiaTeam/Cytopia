#include <filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

std::string FileSystem::readStringFromFile(const std::string &fileName)
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
