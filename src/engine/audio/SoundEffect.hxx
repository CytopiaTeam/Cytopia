#ifndef SOUNDEFFECT_HXX_
#define SOUNDEFFECT_HXX_

#include <string>

#include <SDL.h>
#include <SDL_mixer.h>

#include "AL/al.h"
#include "AL/alc.h"

/** \brief Sound effect class
  * Class for sound effects.
  * Loads and plays sound effects of following formats: WAV, AIFF, RIFF, OGG, and VOC
  */
class SoundEffect
{
public:
  SoundEffect(const std::string &filename);
  SoundEffect() = default;
  ~SoundEffect();

  /** \brief Load File
    * Loads a sound effect file.
    * Supports WAV, AIFF, RIFF, OGG, and VOC
    * @param filename Filename of the music file that should be loaded
    */
  void loadFile(const std::string &filename);

  /** \brief Play sound effect
    * Plays the loaded sound effect file.
    * @param channel Channel number to register this effect on. Default: -1  (use any channel)
    * @param angle Direction in relation to forward from 0 to 360 degrees. Larger angles will be reduced to this range using angles % 360.
                   0 = directly in front.
                   90 = directly to the right.
                   180 = directly behind.
                   270 = directly to the left.
    * @param distance The distance from the listener, from 0(near/loud) to 255(far/quiet).
    * @param loops How many loops should be played. Default: 0 (play one time)
    */
  void play(int channel = -1, Sint16 angle = 0, Uint8 distance = 0, int loops = 0) const;

  /** \brief Stop sound effect
    * Stops the soundeffect at given channel
    * @param channel that should be stopped. Default: -1 (all channels)
    */
  void stop(int channel = -1) const;

  /** \brief Check if a sound effect is playing
    * Checks if this audio object is currently playing.
    * @returns bool if a sound effect is playing
    */
  bool isPlaying() const;

  void enableSoundEffects(bool enabled);

private:
  Mix_Chunk *m_soundEffect = nullptr;

  bool m_playSoundEffect;
  
  ALuint source;
};

#endif
