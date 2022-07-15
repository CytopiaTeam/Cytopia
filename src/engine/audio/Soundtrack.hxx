#ifndef SOUNDTRACK_HXX_
#define SOUNDTRACK_HXX_

#include <memory>
#include <string>

#include "../../util/Meta.hxx"

#include "AL/al.h"
#include "AL/alc.h"

#include <vector>

using std::string;
using ChannelID = StrongType<int, struct ChannelIDTag>;
using SoundtrackID = StrongType<string, struct SoundtrackIDTag>;
using RepeatCount = StrongType<uint8_t, struct RepeatCountTag>;

/**
   * @brief Container for raw pcm data that read from .ogg sound file
   * @details raw pcm data is held in a vector of type char.
   * @param nBytes length of sample in bytes
   * @param data_sample_rate sample rate of audio sample assuming it is just 1 sample rate
   */
struct DecodedAudioData
{
  std::vector<char> char_data_vec; ///< pcm audio data
  long nBytes;                     ///< number of bytes in decoded audio data
  int data_sample_rate;
};

struct Soundtrack
{
  SoundtrackID ID;

  /**
   * @brief The ChannelID
   */
  ChannelID Channel;

  /**
   * @brief The number of times this track should be repeated
   * @details Must be between [0, 255]. A value of 0 will only play once
   *          while a value of 255 will play forever
   */
  RepeatCount Loop;

  /**
   * @brief true if the Soundtrack is Music, false if it's a Sound
   */
  bool isMusic : 1;

  /**
   * @brief true if the Soundtrack is currently playing
   */
  bool isPlaying : 1;

  /**
   * @brief true if the Soundtrack can be played by AudioTrigger
   */
  bool isTriggerable : 1;

  /**
   * @brief true if the Soundtrack can be played by SoundtrackID
   */
  bool isPlayable : 1;

  /**
   * @brief The OpenAL sources of the sound track
   * @details An object that tells the OpenAL system where the sound making object is located in 3D space
   * and what buffer(sound) it makes. source[0] is for sound effects and source[1] is for music.
   */
  ALuint source[2];

  /**
   * @brief The OpenAL buffer of the sound track
   * @details An object that tells the OpenAL system what the sound made is. Must be connected to a source
   * to tell the system where the sound is made.
   */
  ALuint buffer;

  /**
   * @brief The OpenAL effect slot of the sound track
   * @details An object that tells the OpenAL system what effect to apply to the sound. Must be connected to a source
   * to tell the system where the sound is made.
   */
  ALuint effect_slot;

  /**
   * @brief The audio data of the Soundtrack
   */
  DecodedAudioData dAudioDataBuffer;

  Soundtrack(SoundtrackID, ChannelID, DecodedAudioData *, RepeatCount, bool, bool, bool, bool);
  ~Soundtrack();
};

using SoundtrackUPtr = std::unique_ptr<Soundtrack>;

#endif
