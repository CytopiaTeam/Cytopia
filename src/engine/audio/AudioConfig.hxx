#ifndef AUDIO_CONFIG_HXX
#define AUDIO_CONFIG_HXX

#include "Soundtrack.hxx"

#include <unordered_map>
#include <vector>
#include <betterEnums.hxx>

template <typename Key, typename Value> using Mapping = std::unordered_map<Key, Value>;
using string = std::string;
template <typename Type> using Vector = std::vector<Type>;


/**
 * @brief the types of triggers
 */
BETTER_ENUM(AudioTrigger, int, MainTheme)

  struct AudioConfig
{
  struct SoundtrackConfiguration
  {
    string stereoFilePath;
    string monoFilePath;
    Vector<AudioTrigger> triggers;
  };
  Mapping<string, SoundtrackConfiguration> Music;
  Mapping<string, SoundtrackConfiguration> Sound;
};

#endif
