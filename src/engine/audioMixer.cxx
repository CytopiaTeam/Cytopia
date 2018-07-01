#include "audioMixer.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Settings::Instance().settings.audioChannels, 1024);
  setMusicVolume(Settings::Instance().settings.musicVolume);
}

void AudioMixer::setMusicVolume(int volume)
{
  Mix_VolumeMusic(volume);
  Settings::Instance().settings.musicVolume = volume;
}

void AudioMixer::setSoundEffectVolume(int volume)
{
  //Mix_VolumeMusic(volume);
  Settings::Instance().settings.soundEffectsVolume = volume;
}

void AudioMixer::playMusic()
{

  // TODO: stored audio elements should be moved to the resources class after refactoring
  std::shared_ptr<Music> themeMusic(new Music);
  musicObjects.push_back(themeMusic);

  musicObjects[0]->loadFile("resources/audio/music/Theme - Easy Listening.mp3");
  musicObjects[0]->play(-1);
}

void AudioMixer::mute()
{
  //TODO: stop all music stored in the element class after refactoring
  for (auto it : musicObjects)
  {
    it->stop();
    Settings::Instance().settings.playMusic = false;
    Settings::Instance().settings.playSoundEffects = false;
  }
}