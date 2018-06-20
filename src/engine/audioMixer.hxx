#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include "audio/music.hxx"
#include "audio/soundEffect.hxx"
#include "basics/resources.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

class AudioMixer
{
public:
  AudioMixer();
  ~AudioMixer() = default;

  void setMusicVolume(int volume);
  void setSoundEffectVolume(int volume);
  void playMusic();

  void mute();

private:
  std::vector<std::shared_ptr<Music>> musicObjects;
};

#endif