#include "SoundEffect.hxx"

#include "../basics/LOG.hxx"
#include "../basics/Settings.hxx"

SoundEffect::SoundEffect(const std::string &filename) : m_playSoundEffect(Settings::instance().playSoundEffects)
{
  loadFile(filename);
}

SoundEffect::~SoundEffect()
{
  if (m_soundEffect)
  {
    Mix_FreeChunk(m_soundEffect);
  }
}

void SoundEffect::loadFile(const std::string &filename)
{
  m_soundEffect = Mix_LoadWAV(filename.c_str());
  if (!m_soundEffect)
  {
    LOG(LOG_ERROR) << "Failed to load audio file " << filename << "\n" << Mix_GetError();
    LOG() << "Disabled sound effect playback!";
    m_playSoundEffect = false;
  }
}

void SoundEffect::play(int channel, Sint16 angle, Uint8 distance, int loops) const
{
  if (m_playSoundEffect)
  {
    if (m_soundEffect)
    {
      const int currentChannel = Mix_PlayChannel(channel, m_soundEffect, loops);

      if (currentChannel == -1)
      {
        LOG(LOG_ERROR) << "Failed to play audio file\n" << Mix_GetError();
      }

      // for stereo set the position of the sound effect
      if (Settings::instance().audioChannels == 2)
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
  if (m_playSoundEffect)
  {
    Mix_HaltChannel(channel);
  }
}

bool SoundEffect::isPlaying() const
{
  if (m_playSoundEffect)
  {
    // returns amount of playing audiochannels
    return Mix_Playing(-1) != 0;
  }
  return false;
}

void SoundEffect::enableSoundEffects(bool enabled) { m_playSoundEffect = enabled; }
