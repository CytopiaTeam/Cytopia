#include <Filesystem.hxx>
#include <Settings.hxx>
#include <LOG.hxx>
#include <fstream>

#include <SDL.h>

namespace fs
{

std::string readFileAsString(const std::string &fileName, bool binaryMode)
{
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
}

void writeStringToFile(const std::string &fileName, const std::string &stringToWrite, bool binaryMode)
{
  throw()
    LOG() << "fs::writeStringToFile() not implemented!";
}

directory_iterator getDirectoryListing(const std::string &directory) { LOG() << "fs::getDirectoryListing() not implemented!"; }

std::vector<path> getSaveGamePaths() { LOG() << "fs::getSaveGamePaths() not implemented!"; }

bool fileExists(const std::string &filePath) { return exists(path(filePath)); }

std::string getBasePath() { return ""; }

} // namespace fs
