#include "AudioMixer.hxx"

#include "basics/Settings.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Settings::instance().audioChannels, 1024);
  setMusicVolume(Settings::instance().musicVolume);
}

void AudioMixer::setMusicVolume(int volume)
{
  Mix_VolumeMusic(volume);
  Settings::instance().musicVolume = volume;
}

void AudioMixer::setSoundEffectVolume(int volume)
{
  //Mix_VolumeMusic(volume);
  Settings::instance().soundEffectsVolume = volume;
}

void AudioMixer::playMusic()
{
  // TODO: stored audio elements should be moved to the resources class after refactoring
  const std::shared_ptr<Music> themeMusic(new Music);
  m_musicObjects.push_back(themeMusic);

  std::string filePath = SDL_GetBasePath();
  filePath = filePath + "resources/audio/music/Cytopia OST 1.mp3";
  m_musicObjects[0]->loadFile(filePath);
  m_musicObjects[0]->play(-1);
}

void AudioMixer::mute()
{
  //TODO: stop all music stored in the element class after refactoring
  for (const auto &it : m_musicObjects)
  {
    it->stop();
    Settings::instance().playMusic = false;
    Settings::instance().playSoundEffects = false;
  }
}
