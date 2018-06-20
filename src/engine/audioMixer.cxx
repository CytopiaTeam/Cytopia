#include "audioMixer.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Resources::settings.audioChannels, 1024);
  setMusicVolume(Resources::settings.musicVolume);
  //Mix_Music *music = Mix_LoadMUS("resources/audio/music/Theme - Easy Listening.mp3");
  //Mix_PlayMusic(music, 1);
  //Mix_FreeMusic(music);

}

void AudioMixer::setMusicVolume(int volume)
{
  Mix_VolumeMusic(volume);
  Resources::settings.musicVolume=volume;
}

void AudioMixer::setSoundEffectVolume(int volume)
{
  //Mix_VolumeMusic(volume);
  Resources::settings.soundEffectsVolume = volume;
}

void AudioMixer::playMusic()
{

  // TODO: stored audio elements should be moved to the resources class after refactoring
  std::shared_ptr<Music> themeMusic (new Music);
  musicObjects.push_back(themeMusic);

  musicObjects[0]->loadMusic("resources/audio/music/Theme - Easy Listening.mp3");
  musicObjects[0]->play(-1);
}

void AudioMixer::mute()
{
  //TODO: stop all music stored in the element class after refactoring
  for (auto it : musicObjects)
  {
    it->stop();
    Resources::settings.playMusic = false;
    Resources::settings.playSoundEffects = false;
  }
}