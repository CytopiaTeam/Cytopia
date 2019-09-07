#ifndef AUDIOMIXER_HXX_
#define AUDIOMIXER_HXX_

#include <array>
#include <unordered_map>
#include <vector>
#include <memory>
#include <list>

#include "audio/Soundtrack.hxx"
#include "../GameService.hxx"
#include "../util/Meta.hxx"


#ifdef USE_OPENAL_SOFT
#include "AL/al.h"
#include "AL/alc.h"
#endif
=======
#include <thread>

template <typename Key, typename Value> 
using Mapping = std::unordered_map<Key, Value>;
template <typename Type, size_t N>
using Array = std::array<Type, N>;
using string = std::string;
template <typename Type>
using Vector = std::vector<Type>;
template<typename Type>
using Set = std::unordered_set<Type>;
template <typename Type>
using List = std::list<Type>;

struct AudioConfig
{
  struct SoundtrackConfiguration
  {
    string filePath;
    Vector<AudioTrigger> triggers;
  };
  Mapping<string, SoundtrackConfiguration> Music;
  Mapping<string, SoundtrackConfiguration> Sound;
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
  void play(SoundtrackID&& ID) noexcept;

  /**
   * @brief Plays a random Soundtrack from a trigger
   * @param trigger the AudioTrigger
   */
  void play(AudioTrigger&& trigger) noexcept;

  /**
   * @brief Plays a 3D Soundtrack given its ID
   * @param ID the SoundtrackID
   * @param position the Coordinate3D position of the sound
   */
  void play(SoundtrackID&& ID, Coordinate3D&& position) noexcept;

  /**
   * @brief Plays a 3D Soundtrack from a trigger
   * @param trigger the AudioTrigger
   * @param position the Coordinate3D position of the sound
   */
  void play(AudioTrigger&& trigger, Coordinate3D&& position) noexcept;

  /**
   * @brief stops all sounds
   * @param isMuted is muted
   */
  void setMuted(bool isMuted) noexcept;

  AudioMixer(GameService::ServiceTuple&);
  ~AudioMixer();

  /**
   * @brief Loads all game sounds, can be called in new thread
   */
  void loadAllSounds();

  /**
   * @brief joins the thread used to load sounds, if its still running
   *        This must be called when all other threads are joining
   *        when the application is closing, or else it won't close nicely.
   */
  void joinLoadThread();

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
  Mapping<SoundtrackID, SoundtrackUPtr*> m_GetSound;

  /**
   * @brief All the currently playing Soundtracks
   */
  List<SoundtrackUPtr*> m_Playing;

  /**
   * @brief A separate thread for loading the sounds.
   */
  std::thread m_LoadSoundThread;

  /**
   * @brief if this becomes false then loading will stop.
   *        it will be made false by the joinLoadThread function.
   */
  bool running = true;

  /* Event handlers */
  void handleEvent(const AudioTriggerEvent&& event);
  void handleEvent(const AudioTrigger3DEvent&& event);
  void handleEvent(const AudioPlayEvent&& event);
  void handleEvent(const AudioPlay3DEvent&& event);
  void handleEvent(const AudioSoundVolumeChangeEvent&& event);
  void handleEvent(const AudioMusicVolumeChangeEvent&& event);
  void handleEvent(const AudioSetMutedEvent&& event);

  /* Helpers */
  
  /**
   * @brief Plays the Soundtrack
   * @param soundtrack the Soundtrack
   */
  void playSoundtrack(SoundtrackUPtr& soundtrack);

  /**
   * @brief Loads all sounds into the AudioMixer
   * @tparam Iterator an iterator to a Tuple<string, SoundtrackConfiguration>
   * @tparam CallbackType a functor with signature (Soundtrack)(string, Mix_Chunk*)
   * @param begin Iterator to the first item
   * @param end Iterator to after the last item
   * @param createSoundtrack The callback object
   */
  template <typename Iterator, typename CallbackType>
  void loadSoundtrack(Iterator begin, Iterator end, 
      CallbackType createSoundtrack = {});

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
   *          pointer. This means no lambda, no std::function, no std::bind. Our workaround this
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
  ALCdevice* gAudioDevice; 
  
  /**
   * @brief OpenAL Soft setup, context of where audio is played
   */
  ALCcontext* alContext; 
  
  #endif

};

#endif
