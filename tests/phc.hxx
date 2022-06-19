#pragma once

#include <memory>
#include <string>
#include <regex>
#include <future>
#include <thread>
#include <vector>

#include <json.hxx>
#include <betterEnums.hxx>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#ifdef USE_AUDIO
#include <AL/al.h>
#include <AL/alc.h>

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/vorbisenc.h"
#endif

#ifdef USE_ANGELSCRIPT
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#endif

#ifdef USE_MOFILEREADER
#include "moFileReader.h"
#endif

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif