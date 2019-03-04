#include "music.hxx"

#include "../basics/log.hxx"
#include "../basics/settings.hxx"

Music::Music(const std::string &fileName) { loadFile(fileName); }

Music::~Music()
{
  if (m_music)
  {
    Mix_FreeMusic(m_music);
  }
}

void Music::loadFile(const std::string &filename)
{
  m_music = Mix_LoadMUS(filename.c_str());
  if (!m_music)
  {
    LOG(LOG_ERROR) << "Failed to load audio file " << filename << "\n" << Mix_GetError();
  }
}

void Music::play(int loops) const
{
  if (Settings::instance().settings.playMusic)
  {
    if (m_music)
    {
      // stop the music playback first to start from the beginning
      stop();

      if (Mix_PlayMusic(m_music, loops) == -1)
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

void Music::stop() const
{
  if (Settings::instance().settings.playMusic)
  {
    // Reset the music file to the beginning
    Mix_HookMusic(nullptr, nullptr);
    Mix_HaltMusic();
  }
}

void Music::pause() const
{
  if (Settings::instance().settings.playMusic)
  {
    Mix_PauseMusic();
  }
}

void Music::resume() const
{
  if (Settings::instance().settings.playMusic)
  {
    Mix_ResumeMusic();
  }
}

bool Music::isPlaying() const
{
  if (Settings::instance().settings.playMusic)
  {
    // returns amount of playing audiochannels
    return Mix_Playing(-1) != 0;
  }
  return false;
}
