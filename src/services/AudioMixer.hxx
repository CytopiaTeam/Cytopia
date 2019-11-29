#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include <array>
#include <unordered_map>
#include <vector>
#include <memory>
#include <list>

#include "audio/Soundtrack.hxx"
#include "audio/AudioConfig.hxx"
#include "../GameService.hxx"
#include "../util/Meta.hxx"

#ifdef USE_OPENAL_SOFT
#include "AL/al.h"
#include "AL/alc.h"
#endif

template <typename Key, typename Value> using Mapping = std::unordered_map<Key, Value>;
template <typename Type, size_t N> using Array = std::array<Type, N>;
using string = std::string;
template <typename Type> using Vector = std::vector<Type>;
template <typename Type> using Set = std::unordered_set<Type>;
template <typename Type> using List = std::list<Type>;


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
  void setMusicVolume(VolumeLevel volume) noexcept;

  /**
   * @brief sets the sound effects volume
   * @pre   volume must be within [0, 128]
   * @post  Settings::SoundEffectsVolume is changed
   */
  void setSoundEffectVolume(VolumeLevel volume) noexcept;

  /**
   * @brief Plays a Soundtrack given its ID
   * @param ID the SoundtrackID
   */
  void play(SoundtrackID &&ID) noexcept;

  /**
   * @brief Plays a random Soundtrack from a trigger
   * @param trigger the AudioTrigger
   */
  void play(AudioTrigger &&trigger) noexcept;

  /**
   * @brief Plays a 3D Soundtrack given its ID
   * @param ID the SoundtrackID
   * @param position the Coordinate3D position of the sound
   */
#ifdef USE_OPENAL_SOFT
  void play(SoundtrackID &&ID, Coordinate3D &&position) noexcept;
#endif

/**
   * @brief Plays a 3D Soundtrack from a trigger
   * @param trigger the AudioTrigger
   * @param position the Coordinate3D position of the sound
   */
#ifdef USE_OPENAL_SOFT
  void play(AudioTrigger &&trigger, Coordinate3D &&position) noexcept;
#endif
  /**
   * @brief stops all sounds
   * @param isMuted is muted
   */
  void setMuted(bool isMuted) noexcept;

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

  /* Event handlers */
  void handleEvent(const AudioTriggerEvent &&event);
#ifdef USE_OPENAL_SOFT
  void handleEvent(const AudioTrigger3DEvent &&event);
#endif
  void handleEvent(const AudioPlayEvent &&event);
#ifdef USE_OPENAL_SOFT
  void handleEvent(const AudioPlay3DEvent &&event);
#endif
  void handleEvent(const AudioSoundVolumeChangeEvent &&event);
  void handleEvent(const AudioMusicVolumeChangeEvent &&event);
  void handleEvent(const AudioSetMutedEvent &&event);

  /* Helpers */

  /**
   * @brief Plays the Soundtrack
   * @param soundtrack the Soundtrack
   */
  void playSoundtrack(SoundtrackUPtr &soundtrack);

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

  friend class Game;

//openal soft stuff
#ifdef USE_OPENAL_SOFT

  //OpenAL Soft sound setup variables
  /**
   * @brief OpenAL Soft setup, audio device to be used
   */
  ALCdevice *gAudioDevice;

  /**
   * @brief OpenAL Soft setup, context of where audio is played
   */
  ALCcontext *alContext;

#endif
};

#endif
