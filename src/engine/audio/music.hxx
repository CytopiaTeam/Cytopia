#ifndef MUSIC_HXX_
#define MUSIC_HXX_

#include <string>

#include "../basics/log.hxx"
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
  Music(const std::string &fileName);
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
  void play(int loops) const;

  /** \brief stop music
    * Stops the music
    */
  void stop() const;

  /** \brief Pause music
    * Pauses the music
    */
  void pause() const;

  /** \brief Resume music
    * Resumes the music
    */
  void resume() const;

  /** \brief Check if music is playing
    * Checks if this audio object is currently playing music.
    * @returns bool is music playing
    */
  bool isPlaying() const;

private:
  Mix_Music *_music = nullptr;
  Settings settings = Settings::Instance();
};

#endif