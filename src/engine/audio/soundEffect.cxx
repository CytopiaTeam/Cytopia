#include "soundEffect.hxx"

#include "../basics/log.hxx"
#include "../basics/settings.hxx"

SoundEffect::SoundEffect(const std::string &filename) { loadFile(filename); }

SoundEffect::~SoundEffect()
{
  if (_soundEffect)
  {
    Mix_FreeChunk(_soundEffect);
  }
}

void SoundEffect::loadFile(const std::string &filename)
{
  _soundEffect = Mix_LoadWAV(filename.c_str());
  if (!_soundEffect)
  {
    LOG(LOG_ERROR) << "Failed to load audio file " << filename << "\n" << Mix_GetError();
  }
}

void SoundEffect::play(int channel, Sint16 angle, Uint8 distance, int loops) const
{
  if (Settings::instance().settings.playSoundEffects)
  {
    if (_soundEffect)
    {
      int currentChannel = Mix_PlayChannel(channel, _soundEffect, loops);

      if (currentChannel == -1)
      {
        LOG(LOG_ERROR) << "Failed to play audio file\n" << Mix_GetError();
      }

      // for stereo set the position of the sound effect
      if (Settings::instance().settings.audioChannels == 2)
      {
        if (!Mix_SetPosition(currentChannel, angle, distance))
        {
          LOG(LOG_ERROR) << "Failed to set audio position!\n" << Mix_GetError();
        }
      }
    }
    else
    {
      LOG(LOG_ERROR) << "No sound effect file is loaded but the function play() has been called!\n";
    }
  }
}

void SoundEffect::stop(int channel) const
{
  if (Settings::instance().settings.playMusic)
  {
    Mix_HaltChannel(channel);
  }
}

bool SoundEffect::isPlaying() const
{
  if (Settings::instance().settings.playMusic)
  {
    // returns amount of playing audiochannels
    return Mix_Playing(-1) != 0;
  }
  return false;
}
