#include "audioMixer.hxx"

#include "basics/settings.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Settings::instance().settings.audioChannels, 1024);
  setMusicVolume(Settings::instance().settings.musicVolume);
}

void AudioMixer::setMusicVolume(int volume)
{
  Mix_VolumeMusic(volume);
  Settings::instance().settings.musicVolume = volume;
}

void AudioMixer::setSoundEffectVolume(int volume)
{
  //Mix_VolumeMusic(volume);
  Settings::instance().settings.soundEffectsVolume = volume;
}

void AudioMixer::playMusic()
{
  // TODO: stored audio elements should be moved to the resources class after refactoring
  std::shared_ptr<Music> themeMusic(new Music);
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
    Settings::instance().settings.playMusic = false;
    Settings::instance().settings.playSoundEffects = false;
  }
}