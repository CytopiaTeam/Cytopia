#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include <array>
#include <unordered_set>
#include <vector>
#include <memory>
#include <list>

#include "audio/Soundtrack.hxx"
#include "audio/AudioConfig.hxx"
#include "../GameService.hxx"
#include "../util/Meta.hxx"

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
 * @brief the volume level
 */
using VolumeLevel = StrongType<uint8_t, struct VolumeLevelTag>;

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
 *
	@param flDensity, AL_REVERB_DENSITY,
	description: coloration of the late reverb, range: 0.0 to 1.0.

	@param flDiffusion, AL_REVERB_DIFFUSION,
	description: echo density in the reverberation decay, range: 0.0 to 1.0

	@param flGain, AL_REVERB_GAIN,
	description: master volume control for the reflected sound, range: 0.0 to 1.0

	@param flGainHF, AL_REVERB_GAINHF,
	description: attenuation it at high frequencies, range: 0.0 to 1.0

	@param flDecayTime, AL_REVERB_DECAY_TIME,
	description: reverberation decay time, range: 0.1 to 20.0

	@param flDecayHFRatio, AL_REVERB_DECAY_HFRATIO,
	description: ratio of high-frequency decay time relative to the time set by Decay Time, range: 0.1 to 2.0

	@param flReflectionsGain, AL_REVERB_REFLECTIONS_GAIN,
	description: controls the overall amount of initial reflections relative to the Gain property, range: 0.0 to 3.16

	@param flReflectionsDelay, AL_REVERB_REFLECTIONS_DELAY,
	description: amount of delay between the arrival time of the direct path from the source to the first reflection from the source, range: 0.0 to 0.3

	@param flLateReverbGain, AL_REVERB_LATE_REVERB_GAIN,
	description: overall amount of later reverberation relative to the Gain property, range: 0.0 to 10.0

	@param flLateReverbDelay, AL_REVERB_LATE_REVERB_DELAY,
	description: begin time of the late reverberation relative to the time of the initial reflection, range: 0.0 to 0.1

	@param flAirAbsorptionGainHF, AL_REVERB_AIR_ABSORPTION_GAINHF,
	description: the distance-dependent attenuation at high frequencies caused by the propagation medium, range: 0.892 to 1.0

	@param flRoomRolloffFactor, AL_REVERB_ROOM_ROLLOFF_FACTOR,
	description: attenuate the reflected sound, range: 0.0 to 10.0
 */

struct StandardReverbProperties
{
  double flDensity = 1.0;
  double flDiffusion = 1.0;
  double flGain = 0.32;
  double flGainHF = 0.89;
  double flDecayTime = 1.49;
  double flDecayHFRatio = 0.83;
  double flReflectionsGain = 0.05;
  double flReflectionsDelay = 0.007;
  double flLateReverbGain = 1.26;
  double flLateReverbDelay = 0.011;
  double flAirAbsorptionGainHF = 0.994;
  double flRoomRolloffFactor = 0.0;
};

/**
  @brief an echo effect


	@param flEchoDelay,AL_ECHO_DELAY,
	description: delay between the original sound and the first ‘tap’, or echo instance,range: 0.0 to 0.207


	@param flEchoLRDelay,AL_ECHO_LRDELAY,
	description: delay between the first ‘tap’ and the second ‘tap’.,range: 0.0 to 0.404


	@param flEchoDamping,AL_ECHO_DAMPING,
	description: amount of high frequency damping applied to each echo, range: 0.0 to 0.99


	@param flEchoFeedback,AL_ECHO_FEEDBACK,
	description: amount of feedback the output signal fed back into the input, range: 0.0 to 1.0


	@param flEchoSpread,AL_ECHO_SPREAD,
	description: how hard panned the individual echoes are, range: -1.0 to 1.0
*/
struct EchoProperties
{
  double flEchoDelay = 0.1;

  double flEchoLRDelay = 0.1;

  double flEchoDamping = 0.5;

  double flEchoFeedback = 0.5;

  double flEchoSpread = -1.0;
};

/**
 * @class AudioMixer
 */
class AudioMixer : public GameService
{
public:
  using DEFAULT_CHANNELS = Constant<4>;

  /**
   * @brief sets the music volume
   * @pre   volume must be within [0, 128]
   * @post  Settings::MusicVolume is changed
   */
  void setMusicVolume(VolumeLevel volume);

  /**
   * @brief sets the sound effects volume
   * @pre   volume must be within [0, 128]
   * @post  Settings::SoundEffectsVolume is changed
   */
  void setSoundEffectVolume(VolumeLevel volume);

  /**
   * @brief Plays a Soundtrack given its ID
   * @param ID the SoundtrackID
   */
  void play(const SoundtrackID &id);

  /**
   * @brief Plays a random Soundtrack from a trigger
   * @param trigger the AudioTrigger
   */
  void play(const AudioTrigger &trigger);

  /**
   * @brief Plays a 3D Soundtrack given its ID
   * @param ID the SoundtrackID
   * @param position the Coordinate3D position of the sound
   */
  void play(const SoundtrackID &id, const Coordinate3D &position);

  /**
   * @brief Plays a Soundtrack given its ID and applies effect to it
   * @param ID the SoundtrackID
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
  AudioMixer(GameService::ServiceTuple &);
  ~AudioMixer();

  //for orientation of listener
  enum class ORIENTATION_INDEX : int
  {
    FORWARD_X = 0,
    FORWARD_Y = 1,
    FORWARD_Z = 2,
    UP_X = 3,
    UP_Y = 4,
    UP_Z = 5
  };

  //for position of listener
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
   * @param properties of reverb effect
   */
  void playSoundtrackWithReverb(SoundtrackUPtr &soundtrack, const StandardReverbProperties &reverb_properties);

  /**
   * @brief Plays the Soundtrack with echo
   * @param soundtrack the Soundtrack
   * @param properties of echo effect
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
