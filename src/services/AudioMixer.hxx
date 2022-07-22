#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include <array>
#include <unordered_set>
#include <vector>
#include <memory>
#include <list>

#include "audio/Soundtrack.hxx"
#include "audio/AudioConfig.hxx"
#include "../util/Meta.hxx"
#include "../util/Singleton.hxx"

#include "AL/al.h"
#include "AL/alc.h"

#include "AL/alext.h" //header for OpenAL Soft
#include "AL/efx.h"
#include "AL/efx-presets.h"

template <typename Key, typename Value> using Mapping = std::unordered_map<Key, Value>;
template <typename Type, size_t N> using Array = std::array<Type, N>;
using string = std::string;
template <typename Type> using Vector = std::vector<Type>;
template <typename Type> using Set = std::unordered_set<Type>;
template <typename Type> using List = std::list<Type>;

/**
 * @brief a 3-dimensional coordinate
 * @param x x coordinate, horizontal position
 * @param y y coordinate, height
 * @param z z coordinate, vertical position
 */
struct Coordinate3D
{
  double x, y, z;
};

/**
 * @class AudioMixer
 */
class AudioMixer : public Singleton<AudioMixer>
{
public:
  using DEFAULT_CHANNELS = Constant<4>;

  /**
   * @brief sets the music volume
   * @param volume
   * @pre   volume must be a float within [0, 1]
   * @post  Settings::MusicVolume is changed
   */
  void setMusicVolume(float volume);

  /**
   * @brief sets the sound effects volume
   * @param volume
   * @pre   volume must be a float within [0, 1]
   * @post  Settings::SoundEffectsVolume is changed
   */
  void setSoundEffectVolume(float volume);

  /**
   * @brief Plays a Soundtrack given its ID and optionally applies an effect
   * @param id the SoundtrackID
   * @param effect the effect to apply. Should be in the form of an AL_EFFECT macro such as AL_EFFECT_REVERB or AL_EFFECT_ECHO.
   */
  void play(const SoundtrackID &id, int effect = AL_EFFECT_NULL);

  /**
   * @brief Plays a random Soundtrack from a trigger and optionally applies an effect
   * @param trigger the AudioTrigger
   * @param effect the effect to apply. Should be in the form of an AL_EFFECT macro such as AL_EFFECT_REVERB or AL_EFFECT_ECHO.
   */
  void play(const AudioTrigger &trigger, int effect = AL_EFFECT_NULL);

  /**
   * @brief Plays a 3D Soundtrack given its ID and optionally applies an effect
   * @param id the SoundtrackID
   * @param position the Coordinate3D position of the sound
   * @param effect the effect to apply. Should be in the form of an AL_EFFECT macro such as AL_EFFECT_REVERB or AL_EFFECT_ECHO.
   */
  void play(const SoundtrackID &id, const Coordinate3D &position, int effect = AL_EFFECT_NULL);

  /**
   * @brief Plays a 3D Soundtrack from a trigger and optionally applies an effect
   * @param trigger the AudioTrigger
   * @param position the Coordinate3D position of the sound
   * @param effect the effect to apply. Should be in the form of an AL_EFFECT macro such as AL_EFFECT_REVERB or AL_EFFECT_ECHO.
   */
  void play(const AudioTrigger &trigger, const Coordinate3D &position, int effect = AL_EFFECT_NULL);

  /**
   * @brief toggles the mute option for sounds
   * @param isMuted true to mute all sounds, false to unmute
   * @throws UnimplementedError Currently throws UnimplementedError on function call
   */
  void setMuted(bool isMuted);

  /**
   * @brief Stops all soundtracks
   */
  void stopAll();

  /**
   * @brief Updates soundtracks that are no longer playing
   */
  void prune();

  /**
   * @pre GameClock must be initialized
   * @throws ConfigurationError if there is a problem opening the audio config file
   * @throws AudioError if OpenAL context cannot be initialized or an OpenAL error occurs
   */
  AudioMixer();
  ~AudioMixer();
  AudioMixer(const AudioMixer &) = delete;
  AudioMixer(AudioMixer &&) = delete;
  AudioMixer &operator=(const AudioMixer &) = delete;
  AudioMixer &operator=(AudioMixer &&) = delete;

  /// orientation of listener
  enum class ORIENTATION_INDEX : int
  {
    FORWARD_X = 0,
    FORWARD_Y = 1,
    FORWARD_Z = 2,
    UP_X = 3,
    UP_Y = 4,
    UP_Z = 5
  };

  /// position of listener
  enum class POSITION_INDEX : int
  {
    X = 0,
    Y = 1,
    Z = 2
  };

private:
  /**
   * @brief All the available soundtracks
   */
  Vector<SoundtrackUPtr> m_Soundtracks;

  /**
   * @brief A Mapping between triggers and SoundtrackID
   */
  Array<Vector<SoundtrackID>, AudioTrigger::_size()> m_Triggers;

  /**
   * @brief A Mapping between SoundtrackID and Soundtrack
   */
  Mapping<SoundtrackID, SoundtrackUPtr *> m_GetSound;

  /**
   * @brief All the currently playing Soundtracks
   */
  List<SoundtrackUPtr *> m_Playing;

  /* Helpers */

  /**
   * @brief Get a descriptive error message from an error code
   */
  const char *get_al_error_msg(ALenum error);

  /**
   * @brief Plays the Soundtrack
   * @param soundtrack the Soundtrack
   * @throws AudioError if track is invalid or its source is uninitialized
   */
  void playSoundtrack(SoundtrackUPtr &soundtrack);

  /**
   * @brief Plays the Soundtrack with a certain effect applied.
   * @details Given the soundtrack, this function will apply the given AL effect to the audio with the default settings.
   * @param soundtrack the Soundtrack
   * @param ALEffect the effect to apply. Should be in the form of an AL_EFFECT macro such as AL_EFFECT_REVERB or AL_EFFECT_ECHO.
   * @throws AudioError if track is invalid, its source is uninitialized, or reverb effect could not be applied
   */
  void playSoundtrackWithEffect(SoundtrackUPtr &soundtrack, int ALEffect);
  /**
   * @brief Called whenever a Channel has finished playing
   * @param channelID the channel that has finished playing
   */
  void onTrackFinished(int channelID);

  /**
   * @details A global closure to capture an AudioMixer object
   */
  static std::function<void(int)> onTrackFinishedFunc;

  /**
   * @details Because of SDL_Mixer's poor design, we are forced to use a function
   *          pointer. This means no lambda, no std::function, no std::bind. Our workaround
   *          involves creating the onTrackFinishedFunc global function object which captures the AudioMixer object
   */
  static void onTrackFinishedFuncPtr(int channelID) { onTrackFinishedFunc(channelID); }

  SoundtrackUPtr &getTrack(const AudioTrigger &trigger);
  SoundtrackUPtr &getTrack(const SoundtrackID &id);

  friend class Game;

  //OpenAL Soft sound setup variables
  /**
   * @brief OpenAL Soft setup, audio device to be used
   */
  ALCdevice *gAudioDevice;

  /**
   * @brief OpenAL Soft setup, context of where audio is played
   */
  ALCcontext *alContext;
};

#endif
