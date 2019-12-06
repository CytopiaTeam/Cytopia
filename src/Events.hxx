#ifndef EVENTS_HXX_
#define EVENTS_HXX_

#include <string>

#include "betterEnums.hxx"
#include "util/Meta.hxx"
#include "util/TransitiveModel.hxx"
#include "engine/audio/Soundtrack.hxx"
#include "engine/audio/AudioConfig.hxx"

using std::string;

/**
 * @brief Events to be handled by the UI Loop
 */
using UIEvents = TypeList<struct TerminateEvent /* Add UI Events here */>;

/**
 * @brief Events to be handled by the Game Loop
 */
using GameEvents = TypeList<struct TerminateEvent,
                            /* All Audio events */
                            struct AudioTriggerEvent, struct AudioTrigger3DEvent, struct AudioPlayEvent, struct AudioPlay3DEvent,
                            struct AudioMusicVolumeChangeEvent, struct AudioSoundVolumeChangeEvent, struct AudioSetMutedEvent,
							struct AudioStopEvent, struct AudioPruneEvent
                            /* Add Game Events here */>;

/**
 * @brief Event triggered when the game must terminate
 */
struct TerminateEvent
{
};

/**
 * @brief Event describing a Widget change
 * @tparam UITargetType the target widget type
 * @tparam UpdateEventType the update type
 */
template <typename UITargetType, typename UpdateEventType> struct UIChangeEvent
{
  UITargetType target;
  UpdateEventType data;
};

/**
 * @brief A Widget transitive change
 * @tparam TransitiveType the transitive type
 */
template <typename TransitiveType>
using TransitiveStateChange =
    UIChangeEvent<ObserverWPtr<typename TransitiveType::Transition>, typename TransitiveType::Transition>;

/* Audio Events */

/**
 * @brief the volume level
 */
using VolumeLevel = StrongType<uint8_t, struct VolumeLevelTag>;

/**
 * @brief a 3-dimensional coordinate
 */
struct Coordinate3D
{
  double x, y, z;
};

/**
 * @brief event triggering a set of tracks
 */
struct AudioTriggerEvent
{
  AudioTrigger trigger;
};

/**
 * @brief event playing a track
 */
struct AudioPlayEvent
{
  SoundtrackID ID;
};

/**
 * @brief event triggering a set of tracks with 3D sound
 */
struct AudioTrigger3DEvent
{
  AudioTrigger trigger;
  Coordinate3D position;
};

/**
 * @brief event playing a track with 3D sound
 */
struct AudioPlay3DEvent
{
  SoundtrackID ID;
  Coordinate3D position;
};

/**
 * @brief event changing the volume level of sounds
 */
struct AudioMusicVolumeChangeEvent
{
  VolumeLevel level;
};

/**
 * @brief event changing the volume level of music
 */
struct AudioSoundVolumeChangeEvent
{
  VolumeLevel level;
};

/**
 * @brief event setting audio to muted
 */
struct AudioSetMutedEvent
{
  bool isMuted;
};

/**
 * @brief event stopping all soundtracks
 */
struct AudioStopEvent
{
};

/**
 * @brief event pruning all stopped soundtracks
 */
struct AudioPruneEvent
{
};

/**
 * @brief All audio-related events
 */
using AudioEvents = TypeList<AudioTriggerEvent, AudioTriggerEvent, AudioTrigger3DEvent, AudioPlayEvent, AudioPlay3DEvent,
                             AudioMusicVolumeChangeEvent, AudioSoundVolumeChangeEvent, AudioSetMutedEvent, AudioStopEvent, AudioPruneEvent>;

#endif
