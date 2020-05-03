#include <filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

std::string FileSystem::readFileAsString(const std::string &fileName, bool binaryMode)
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
#endif
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
