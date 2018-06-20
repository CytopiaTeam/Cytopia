#include "music.hxx"
//#include "basics\resources.hxx"

Music::Music()
{

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
      if (! Mix_PlayMusic(_music, loops))
      {
        LOG(LOG_ERROR) << "Failed to play music!\n" << Mix_GetError();
      }
    }
    else
    {
      LOG(LOG_ERROR) << "No music file loaded but play() is called!\n";
    }
  }
}