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
 * @param x, x coordinate, horizontal position
 * @param y, y coordinate, height
 * @param z, z coordinate, vertical position
 */
struct Coordinate3D
{
  double x, y, z;
};

/**
 * @brief a standard reverb effect
 */

struct StandardReverbProperties
{
  double flDensity = 1.0;       ///< coloration of the late reverb, range: 0.0 to 1.0.
  double flDiffusion = 1.0;     ///< echo density in the reverberation decay, range: 0.0 to 1.0
  double flGain = 0.32;         ///< master volume control for the reflected sound, range: 0.0 to 1.0
  double flGainHF = 0.89;       ///< attenuation it at high frequencies, range: 0.0 to 1.0
  double flDecayTime = 1.49;    ///< reverberation decay time, range: 0.1 to 20.0
  double flDecayHFRatio = 0.83; ///< ratio of high-frequency decay time relative to the time set by Decay Time, range: 0.1 to 2.0
  double flReflectionsGain =
      0.05; ///< controls the overall amount of initial reflections relative to the Gain property, range: 0.0 to 3.16
  double flReflectionsDelay =
      0.007; ///< amount of delay between the arrival time of the direct path from the source to the first reflection from the source, range: 0.0 to 0.3
  double flLateReverbGain = 1.26; ///< overall amount of later reverberation relative to the Gain property, range: 0.0 to 10.0
  double flLateReverbDelay =
      0.011; ///< begin time of the late reverberation relative to the time of the initial reflection, range: 0.0 to 0.1
  double flAirAbsorptionGainHF =
      0.994; ///< the distance-dependent attenuation at high frequencies caused by the propagation medium, range: 0.892 to 1.0
  double flRoomRolloffFactor = 0.0; ///< attenuate the reflected sound, range: 0.0 to 10.0
};

/**
  @brief an echo effect 
*/
struct EchoProperties
{
  double flEchoDelay = 0.1; ///< delay between the original sound and the first 'tap', or echo instance,range: 0.0 to 0.207

  double flEchoLRDelay = 0.1; ///< delay between the first 'tap' and the second 'tap'.,range: 0.0 to 0.404

  double flEchoDamping = 0.5; ///< amount of high frequency damping applied to each echo, range: 0.0 to 0.99

  double flEchoFeedback = 0.5; ///< amount of feedback the output signal fed back into the input, range: 0.0 to 1.0

  double flEchoSpread = -1.0; ///< how hard panned the individual echoes are, range: -1.0 to 1.0
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
   * @brief Plays a Soundtrack given its ID
   * @param id the SoundtrackID
   */
  void play(const SoundtrackID &id);

  /**
   * @brief Plays a random Soundtrack from a trigger
   * @param trigger the AudioTrigger
   */
  void play(const AudioTrigger &trigger);

  /**
   * @brief Plays a 3D Soundtrack given its ID
   * @param id the SoundtrackID
   * @param position the Coordinate3D position of the sound
   */
  void play(const SoundtrackID &id, const Coordinate3D &position);

  /**
   * @brief Plays a Soundtrack given its ID and applies effect to it
   * @param id the SoundtrackID
   * @param reverb_properties properties of standard reverb
   */
  void play(const SoundtrackID &id, const StandardReverbProperties &reverb_properties);

  /**
   * @brief Plays a Soundtrack given its ID and applies effect to it
   * @param id the SoundtrackID
   * @param echo_properties properties of echo
   */
  void play(const SoundtrackID &id, const EchoProperties &echo_properties);

  /**
   * @brief Plays a Soundtrack from a trigger and applies effect to it
   * @param trigger the AudioTrigger
   * @param reverb_properties properties of standard reverb
   */
  void play(const AudioTrigger &trigger, const StandardReverbProperties &reverb_properties);

  /**
   * @brief Plays a Soundtrack from a trigger and applies effect to it
   * @param trigger the AudioTrigger
   * @param echo_properties properties of standard reverb
   */
  void play(const AudioTrigger &trigger, const EchoProperties &echo_properties);

  /**
   * @brief Plays a 3D Soundtrack from a trigger
   * @param trigger the AudioTrigger
   * @param position the Coordinate3D position of the sound
   */

  void play(const AudioTrigger &trigger, const Coordinate3D &position);

  /**
   * @brief Plays a 3D Soundtrack given its ID and applies effect to it
   * @param id the SoundtrackID
   * @param position the Coordinate3D position of the sound
   * @param reverb_properties properties of standard reverb
   */
  void play(const SoundtrackID &id, const Coordinate3D &position, const StandardReverbProperties &reverb_properties);

  /**
   * @brief Plays a 3D Soundtrack given its ID and applies effect to it
   * @param id the SoundtrackID
   * @param position the Coordinate3D position of the sound
   * @param echo_properties properties of echo
   */

  void play(const SoundtrackID &id, const Coordinate3D &position, const EchoProperties &echo_properties);

  /**
   * @brief Plays a 3D Soundtrack from a trigger and applies effect to it
   * @param trigger the AudioTrigger
   * @param position the Coordinate3D position of the sound
   * @param reverb_properties properties of standard reverb
   */
  void play(const AudioTrigger &trigger, const Coordinate3D &position, const StandardReverbProperties &reverb_properties);

  /**
   * @brief Plays a 3D Soundtrack from a trigger and applies effect to it
   * @param trigger the AudioTrigger
   * @param position the Coordinate3D position of the sound
   * @param echo_properties properties of echo
   */
  void play(const AudioTrigger &trigger, const Coordinate3D &position, const EchoProperties &echo_properties);

  /**
   * @brief stops all sounds
   * @param isMuted is muted
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
   */
  void playSoundtrack(SoundtrackUPtr &soundtrack);

  /**
   * @brief Plays the Soundtrack with reverb
   * @param soundtrack the Soundtrack
   * @param reverb_properties of reverb effect
   */
  void playSoundtrackWithReverb(SoundtrackUPtr &soundtrack, const StandardReverbProperties &reverb_properties);

  /**
   * @brief Plays the Soundtrack with echo
   * @param soundtrack the Soundtrack
   * @param echo_properties of echo effect
   */
  void playSoundtrackWithEcho(SoundtrackUPtr &soundtrack, const EchoProperties &echo_properties);

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
