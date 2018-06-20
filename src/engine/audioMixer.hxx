#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include "audio/music.hxx"
#include "basics/resources.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

class AudioMixer
{
public:
  AudioMixer();
  ~AudioMixer() = default;

  void setVolume(int volume);
  void playMusic();

};

#endif