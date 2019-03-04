#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include <vector>
#include <memory>

#include "audio/music.hxx"
#include "audio/soundEffect.hxx"

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
  std::vector<std::shared_ptr<Music>> m_musicObjects;
};

#endif