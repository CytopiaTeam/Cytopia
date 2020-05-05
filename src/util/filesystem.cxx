#include <filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

namespace fs
{

std::string readFileAsString(const std::string &fileName, bool binaryMode)
{
#ifdef __ANDROID__
  // on Android, files cannot be accessed directly without Java. SDL implements that for us, so we use SDL RWOps until we implemented JNI File Access ourselves
  // binary mode is ignored for now, savegames can't be loaded

  SDL_RWops *rw = SDL_RWFromFile(fileName.c_str(), "rb");
  Sint64 res_size = SDL_RWsize(rw);
  char *res = (char *)malloc(res_size + 1);
  if (rw == NULL)
  {
    LOG(LOG_ERROR) << stderr << "Couldn't open " << fileName.c_str();
  }

  Sint64 nb_read_total = 0, nb_read = 1;
  char *buf = res;
  while (nb_read_total < res_size && nb_read != 0)
  {
    nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
    nb_read_total += nb_read;
    buf += nb_read;
  }

  res[nb_read_total] = '\0';

  std::string i = res;
  SDL_RWclose(rw);
  if (nb_read_total != res_size)
  {
    free(res);
    LOG(LOG_ERROR) << "ERROR loading file " << fileName.c_str();
  }

  return res;
#else
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
#endif
}

void writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode)
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

directory_iterator getDirectoryListing(const std::string &directory)
{
  std::string pathToSaveFiles = getBasePath();
  pathToSaveFiles.append(directory);
  return directory_iterator(std::filesystem::path(pathToSaveFiles));
}

std::vector<path> getSaveGamePaths()
{
  std::vector<path> saveGames;
  forEachFileType("resources", ".cts", [&saveGames](const auto &path) { saveGames.emplace_back(path); });
  return saveGames;
}

bool fileExists(const std::string &filePath) { return exists(path(filePath)); }

std::string getBasePath()
{
  std::string sPath;

#ifndef __ANDROID__
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
#endif

  return sPath;
}

} // namespace fs
