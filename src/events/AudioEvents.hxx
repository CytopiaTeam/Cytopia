#ifndef AUDIO_EVENTS_HXX
#define AUDIO_EVENTS_HXX

#include "../engine/audio/Soundtrack.hxx"
#include "../engine/audio/AudioConfig.hxx"

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
using AudioEvents = TypeList<AudioTriggerEvent, 
  AudioTriggerEvent, 
  AudioTrigger3DEvent, 
  AudioPlayEvent, 
  AudioPlay3DEvent,
  AudioMusicVolumeChangeEvent, 
  AudioSoundVolumeChangeEvent, 
  AudioSetMutedEvent, 
  AudioStopEvent, 
  AudioPruneEvent
>;

#endif
