#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include <vector>
#include <memory>
#include <iostream>

#include "audio/Music.hxx"
#include "audio/SoundEffect.hxx"

#ifdef USE_OPENAL_SOFT
#include "AL/al.h"
#include "AL/alc.h"
#endif

/**
 * @class AudioMixer
 * @todo this should be a singleton
 */
class AudioMixer
{
public:
  AudioMixer();
  ~AudioMixer();

  /**
   * @brief sets the music volume
   * @pre   volume must be within [0, 128]
   * @post  Settings::MusicVolume is changed
   */
  void setMusicVolume(uint8_t volume) noexcept;

  /**
   * @brief sets the sound effects volume
   * @pre   volume must be within [0, 128]
   * @post  Settings::SoundEffectsVolume is changed
   * @todo  this doesn't do anything. must find a way to
   *        actually deal with SDL2_Mixer
   */
  void setSoundEffectVolume(uint8_t volume) noexcept;

  /**
   * @brief starts theme music
   * @todo  stored audio elements should be moved 
   *        to the resources class
   */
  void playMusic(void) noexcept;

  /**
   * @brief stops all sounds
   * @todo  stop all music stored in the element class 
   */
  void mute(void) noexcept;

private:
  std::vector<std::shared_ptr<Music>> m_musicObjects;
  
    //openal soft stuff
  #ifdef USE_OPENAL_SOFT
  //OpenAL Soft sound setup variables
  ALCdevice* gAudioDevice; //audio device to be used
  ALCcontext* alContext; //context of where audio is played
  #endif
};

#endif
