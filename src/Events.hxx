#ifndef EVENTS_HXX_
#define EVENTS_HXX_

#include <string>

#include "betterEnums.hxx"
#include "util/Meta.hxx"
#include "util/TransitiveModel.hxx"
#include "engine/audio/Soundtrack.hxx"

using std::string;

using UIEvents = TypeList<struct TerminateEvent /* Add UI Events here */ >;
using GameEvents = TypeList<
  struct TerminateEvent, 
  /* All Audio events */
  struct AudioTriggerEvent,
  struct AudioTrigger3DEvent,
  struct AudioPlayEvent, 
  struct AudioPlay3DEvent, 
  struct AudioMusicVolumeChangeEvent, 
  struct AudioSoundVolumeChangeEvent, 
  struct AudioSetMutedEvent 
  /* Add Game Events here */>;

/**
 * @brief Event triggered when the game must terminate
 */
struct TerminateEvent { };

template <typename UITargetType, typename UpdateEventType>
struct UIChangeEvent 
{
  UITargetType target;
  UpdateEventType data;
};

template <typename TransitiveType>
using TransitiveStateChange = UIChangeEvent<ObserverWPtr<typename TransitiveType::Transition>, typename TransitiveType::Transition>;

/*
 +----------------+
 |  Audio Events  |
 +----------------+
 */

BETTER_ENUM(AudioTrigger, int, MainTheme)
using VolumeLevel = StrongType<uint8_t, struct VolumeLevelTag>;

struct Coordinate3D { int x, y, z; };

struct AudioTriggerEvent
{
  AudioTrigger trigger;
};

struct AudioPlayEvent
{
  SoundtrackID ID;
};

struct AudioTrigger3DEvent
{
  AudioTrigger trigger;
  Coordinate3D position;
};

struct AudioPlay3DEvent
{
  SoundtrackID ID;
  Coordinate3D position;
};

struct AudioMusicVolumeChangeEvent
{
  VolumeLevel level;
};

struct AudioSoundVolumeChangeEvent
{
  VolumeLevel level;
};

struct AudioSetMutedEvent
{
  bool isMuted;
};

#endif
