#include "music.hxx"
//#include "basics\resources.hxx"

Music::Music()
{
  //Mix_FreeMusic(music);
  LOG() << "CTr";

}

Music::~Music()
{
  if (_music)
  {
    Mix_FreeMusic(_music);
  }
}

void Music::loadMusic(const std::string &filename)
{
  LOG() << "LOADING";
  _music = Mix_LoadMUS(filename.c_str());
  if (!_music)
  {
    LOG(LOG_ERROR) << "Failed to load audio file " << filename << "\n" << Mix_GetError();
  }


}

void Music::play(int loops)
{
  if (Resources::settings.playMusic)
  {
    if (_music)
    {
      Mix_PlayMusic(_music, loops);
    }
    else
    {
      LOG(LOG_ERROR) << "No music file loaded but play() is called!";
    }
  }
}