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
