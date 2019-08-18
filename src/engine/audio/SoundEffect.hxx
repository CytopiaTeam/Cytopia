#ifndef SOUNDEFFECT_HXX_
#define SOUNDEFFECT_HXX_

#include <string>

#include <SDL.h>
#include <SDL_mixer.h>

#ifdef USE_OPENAL_SOFT
#include "AL/al.h"
#include "AL/alc.h"
#include <cassert>
#endif

/** \brief Sound effect class
  * Class for sound effects.
  * Loads and plays sound effects of following formats: WAV, AIFF, RIFF, OGG, and VOC
  */
class SoundEffect
{
public:
  SoundEffect(const std::string &filename);
  SoundEffect() = default;
 
  /**
    * @brief Destroys the SoundEffect object
    */
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
  
   /** \brief Play loaded sound effect in 3d sound. Assume listener is facing forward which is perpendicular to 
    * up direction of game world. Up game world direction = +y in openal, isometric xy = openal xz plane.
    * @param x isometric coordinate to describe horizontal position of object where sound originates
    * @param y isometric coordinate to describe vertical position of where sound originates
    * @param elevation How low or high up an object is. Pseudo Up direction axis of game world. 
				0 = very bottom, lowest floor
				1 = 
				2 = 
				3 = 
				4 = starting ground level
				5 = 
				6 = 
				7 = very top, highest floor
    * @param loops How many loops should be played. Default: 0 (play one time)
    */
  #ifdef USE_OPENAL_SOFT  
  void play3DSound(int x, int y, int elevation, int loops = 0);
  #endif
  
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
  
  #ifdef USE_OPENAL_SOFT
  ALuint source;
  ALuint buffer;
  #endif
  
};

#endif
