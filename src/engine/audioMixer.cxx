#include "audioMixer.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Resources::settings.audioChannels, 1024);
  setVolume(Resources::settings.musicVolume);
  //Mix_Music *music = Mix_LoadMUS("resources/audio/music/Theme - Easy Listening.mp3");
  //Mix_PlayMusic(music, 1);
  //Mix_FreeMusic(music);

}

void AudioMixer::setVolume(int volume)
{
  Mix_VolumeMusic(volume);
}

void AudioMixer::playMusic()
{
  std::shared_ptr<Music> themeMusic (new Music);
  musicObjects.push_back(themeMusic);

  musicObjects[0]->loadMusic("resources/audio/music/Theme - Easy Listening.mp3");
  musicObjects[0]->play(-1);
}
