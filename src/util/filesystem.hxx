#ifndef FILESYSTEM_HXX_
#define FILESYSTEM_HXX_

#include <ciso646>
#include "SDL2/SDL.h"

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#else
#pragma message("Warning: Your compiler does not support the filesystem library. Please report this incident to the Cytopia Team")
#endif

namespace fs
{
#if __has_include(<filesystem>)
using namespace std::filesystem;
#elif __has_include(<experimental/filesystem>)
using namespace std::experimental::filesystem;
#endif

std::string getBasePath() noexcept;

}; // namespace fs

#endif // FILESYSTEM_HXX_
