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
	//AL_EAXREVERB_DENSITY, 
	double flDensity;
	//AL_EAXREVERB_DIFFUSION, 
	double flDiffusion;
	//AL_EAXREVERB_GAIN, 
	double flGain;
	//AL_EAXREVERB_GAINHF, 
	double flGainHF;
	//AL_EAXREVERB_GAINLF, 
	double flGainLF;
	//AL_EAXREVERB_DECAY_TIME, 
	double flDecayTime;
	//AL_EAXREVERB_DECAY_HFRATIO, 
	double flDecayHFRatio;
	//AL_EAXREVERB_DECAY_LFRATIO, 
	double flDecayLFRatio;
	//AL_EAXREVERB_REFLECTIONS_GAIN, 
	double flReflectionsGain;
	//AL_EAXREVERB_REFLECTIONS_DELAY, 
	double flReflectionsDelay;
	//alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, double flReflectionsPan;
	//AL_EAXREVERB_LATE_REVERB_GAIN, 
	double flLateReverbGain;
	//AL_EAXREVERB_LATE_REVERB_DELAY, 
	double flLateReverbDelay;
	//alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, double flLateReverbPan;
	//AL_EAXREVERB_ECHO_TIME, 
	double flEchoTime;
	//AL_EAXREVERB_ECHO_DEPTH, 
	double flEchoDepth;
	//AL_EAXREVERB_MODULATION_TIME, 
	double flModulationTime;
	//AL_EAXREVERB_MODULATION_DEPTH, 
	double flModulationDepth;
	//AL_EAXREVERB_HFREFERENCE, 
	double flHFReference;
	//AL_EAXREVERB_LFREFERENCE, 
	double flLFReference;
	//AL_EAXREVERB_AIR_ABSORPTION_GAINHF, 
	double flAirAbsorptionGainHF;
	//AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, 
	double flRoomRolloffFactor;
	//AL_EAXREVERB_DECAY_HFLIMIT, 
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
