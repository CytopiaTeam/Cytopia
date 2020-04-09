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
 * @brief a standard reverb effect
 */
struct StandardReverbProperties
{
	//AL_REVERB_DENSITY, 
	double flDensity;
	//AL_REVERB_DIFFUSION, 
	double flDiffusion;
	//AL_REVERB_GAIN, 
	double flGain;
	//AL_REVERB_GAINHF, 
	double flGainHF;
	//AL_REVERB_DECAY_TIME, 
	double flDecayTime;
	//AL_REVERB_DECAY_HFRATIO, 
	double flDecayHFRatio;
	//AL_REVERB_REFLECTIONS_GAIN, 
	double flReflectionsGain;
	//AL_REVERB_REFLECTIONS_DELAY, 
	double flReflectionsDelay;
	//AL_REVERB_LATE_REVERB_GAIN, 
	double flLateReverbGain;
	//AL_REVERB_LATE_REVERB_DELAY, 
	double flLateReverbDelay;
	//AL_REVERB_AIR_ABSORPTION_GAINHF, 
	double flAirAbsorptionGainHF;
	//AL_REVERB_ROOM_ROLLOFF_FACTOR, 
	double flRoomRolloffFactor;
	//AL_REVERB_DECAY_HFLIMIT, 
	//int iDecayHFLimit;
	
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
 * @brief event playing a track with reverb effect
 */
struct AudioPlayReverbEvent
{
  SoundtrackID ID;
  StandardReverbProperties reverb_properties;	
};

/**
 * @brief event triggering a track with reverb
 */
struct AudioTriggerReverbEvent
{
  AudioTrigger trigger;
  StandardReverbProperties reverb_properties;
};

/**
 * @brief event playing a track with reverb effect with 3D sound
 */
struct AudioPlayReverb3DEvent
{
  SoundtrackID ID;
  Coordinate3D position;
  StandardReverbProperties reverb_properties;	
};

/**
 * @brief event triggering a track with reverb with 3D sound
 */
struct AudioTriggerReverb3DEvent
{
  AudioTrigger trigger;
  Coordinate3D position;
  StandardReverbProperties reverb_properties;
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
  AudioTriggerReverbEvent,
  AudioTriggerReverb3DEvent, 
  AudioPlayEvent, 
  AudioPlay3DEvent,
  AudioPlayReverbEvent,
  AudioPlayReverb3DEvent,
  AudioMusicVolumeChangeEvent, 
  AudioSoundVolumeChangeEvent, 
  AudioSetMutedEvent, 
  AudioStopEvent, 
  AudioPruneEvent
>;

#endif
