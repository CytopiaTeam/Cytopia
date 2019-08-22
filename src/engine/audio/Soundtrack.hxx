#ifndef SOUNDTRACK_HXX_
#define SOUNDTRACK_HXX_

#include <memory>
#include <SDL_mixer.h>
#include <string>

#include "../../util/Meta.hxx"

using std::string;
using ChannelID = StrongType<int, struct ChannelIDTag>;
using SoundtrackID = StrongType<string, struct SoundtrackIDTag>;
using RepeatCount = StrongType<uint8_t, struct RepeatCountTag>;

struct Soundtrack
{
  SoundtrackID ID;

  /**
   * @brief The ChannelID
   * @
   */
  ChannelID Channel;
  
  /**
   * @brief The WAVE data of the Soundtrack
   */
  Mix_Chunk* Chunks;

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

  ~Soundtrack()
  {
    if(Chunks) delete Chunks;
  }

};

using SoundtrackUPtr = std::unique_ptr<Soundtrack>;

#endif
