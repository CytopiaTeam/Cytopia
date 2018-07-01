#ifndef MUSIC_HXX_
#define MUSIC_HXX_

#include <string>

#include "../basics/log.hxx"
#include "../basics/resources.hxx"
#include "../basics/settings.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

/** \brief Music class
  * Class for playing Music.
  * Loads and plays music of following formats:  WAV, MOD, MIDI, OGG, MP3, FLAC
  */
class Music
{
public:
  Music() = default;
  ~Music();

  /** \brief Load File
    * Loads a music file of WAV, MOD, MIDI, OGG, MP3, FLAC.
    * @param filename Filename of the music file that should be loaded
    */
  void loadFile(const std::string &filename);

  /** \brief Play music
    * Plays the loaded music file.
    * @param loops Amount of loops the track. -1 = infinite
    */
  void play(int loops);

  /** \brief stop music
    * Stops the music
    */
  void stop();

  /** \brief Pause music
    * Pauses the music
    */
  void pause();

  /** \brief Resume music
    * Resumes the music
    */
  void resume();

  /** \brief Check if music is playing
    * Checks if this audio object is currently playing music.
    * @returns bool is music playing
    */
  bool isPlaying();

private:
  Mix_Music *_music = nullptr;
  Settings settings = Settings::Instance();
};

#endif