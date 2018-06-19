#include "audioMixer.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS, 2, 640);
  Mix_Music *music = Mix_LoadMUS("resources/audio/music/Theme - Easy Listening.mp3");
  Mix_PlayMusic(music, 1);
  //Mix_FreeMusic(music);

}