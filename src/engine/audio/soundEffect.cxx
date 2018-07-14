#include "soundEffect.hxx"

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

void SoundEffect::play(int channel, int angle, int distance, int loops)
{
  if (settings.settings.playSoundEffects)
  {
    if (_soundEffect)
    {
      int currentChannel = Mix_PlayChannel(channel, _soundEffect, loops);

      if (currentChannel == -1)
      {
        LOG(LOG_ERROR) << "Failed to play audio file\n" << Mix_GetError();
      }

      // for stereo set the position of the sound effect
      if (settings.settings.audioChannels == 2)
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

void SoundEffect::stop(int channel)
{
  if (settings.settings.playMusic)
  {
    Mix_HaltChannel(-1);
  }
}

bool SoundEffect::isPlaying()
{
  if (settings.settings.playMusic)
  {
    // returns amount of playing audiochannels
    return Mix_Playing(-1) != 0;
  }
  return false;
}
