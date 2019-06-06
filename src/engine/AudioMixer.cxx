#include "AudioMixer.hxx"

#include "basics/Settings.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Settings::instance().settings.audioChannels, 1024);
  setMusicVolume(Settings::instance().settings.musicVolume);
}

void AudioMixer::setMusicVolume(uint8_t volume) noexcept
{
  volume = Mix_VolumeMusic(volume);
  Settings::instance().settings.musicVolume = volume;
}

void AudioMixer::setSoundEffectVolume(uint8_t volume) noexcept
{
  Settings::instance().settings.soundEffectsVolume = volume;
}

void AudioMixer::playMusic(void) noexcept
{
  const std::shared_ptr<Music> themeMusic(new Music);
  m_musicObjects.push_back(themeMusic);

  std::string filePath = SDL_GetBasePath();
  filePath = filePath + "resources/audio/music/Cytopia OST 1.mp3";
  m_musicObjects[0]->loadFile(filePath);
  m_musicObjects[0]->play(-1);
}

void AudioMixer::mute(void) noexcept
{
  for (const auto &it : m_musicObjects)
  {
    it->stop();
    Settings::instance().settings.playMusic = false;
    Settings::instance().settings.playSoundEffects = false;
  }
}
