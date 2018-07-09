#include "music.hxx"
//#include "basics\resources.hxx"

Music::~Music()
{
  if (_music)
  {
    Mix_FreeMusic(_music);
  }
}

void Music::loadFile(const std::string &filename)
{
  _music = Mix_LoadMUS(filename.c_str());
  if (!_music)
  {
    LOG(LOG_ERROR) << "Failed to load audio file " << filename << "\n" << Mix_GetError();
  }
}

void Music::play(int loops)
{
  if (settings.settings.playMusic)
  {
    if (_music)
    {
      // stop the music playback first to start from the beginning
      stop();

      if (Mix_PlayMusic(_music, loops) == -1)
      {
        LOG(LOG_ERROR) << "Failed to play music!\n" << Mix_GetError();
      }
    }
    else
    {
      LOG(LOG_ERROR) << "No music file is loaded but the function play() has been called!\n";
    }
  }
}

void Music::stop()
{
  if (settings.settings.playMusic)
  {
    // Reset the music file to the beginning
    Mix_HookMusic(nullptr, nullptr);
    Mix_HaltMusic();
  }
}

void Music::pause()
{
  if (settings.settings.playMusic)
  {
    Mix_PauseMusic();
  }
}

void Music::resume()
{
  if (settings.settings.playMusic)
  {
    Mix_ResumeMusic();
  }
}

bool Music::isPlaying()
{
  if (settings.settings.playMusic)
  {
    // returns amount of playing audiochannels
    return Mix_Playing(-1) != 0;
  }
  return false;
}
