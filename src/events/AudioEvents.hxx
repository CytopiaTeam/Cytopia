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
  //AL_REVERB_DENSITY, description: coloration of the late reverb,
  //range: 0.0 to 1.0.
  double flDensity = 1.0;

  //AL_REVERB_DIFFUSION,  description: echo density in the reverberation decay,
  //range: 0.0 to 1.0
  double flDiffusion = 1.0;

  //AL_REVERB_GAIN,   description: master volume control for the reflected sound,
  //range: 0.0 to 1.0
  double flGain = 0.32;

  //AL_REVERB_GAINHF,   description: attenuation it at high frequencies,
  //range: 0.0 to 1.0
  double flGainHF = 0.89;

  //AL_REVERB_DECAY_TIME,  description: reverberation decay time,
  //range: 0.1 to 20.0
  double flDecayTime = 1.49;

  //AL_REVERB_DECAY_HFRATIO,  description: ratio of high-frequency decay time relative to the time set by Decay Time,
  // range: 0.1 to 2.0
  double flDecayHFRatio = 0.83;

  //AL_REVERB_REFLECTIONS_GAIN,  description: controls the overall amount of initial reflections relative to the Gain property
  //, range: 0.0 to 3.16
  double flReflectionsGain = 0.05;

  //AL_REVERB_REFLECTIONS_DELAY,  description: amount of delay between the arrival time of the direct path from the source to the first reflection from the source
  //range: 0.0 to 0.3
  double flReflectionsDelay = 0.007;

  //AL_REVERB_LATE_REVERB_GAIN,  description: overall amount of later reverberation relative to the Gain property
  //range: 0.0 to 10.0
  double flLateReverbGain = 1.26;

  //AL_REVERB_LATE_REVERB_DELAY,  description: begin time of the late reverberation relative to the time of the initial reflection
  //range: 0.0 to 0.1
  double flLateReverbDelay = 0.011;

  //AL_REVERB_AIR_ABSORPTION_GAINHF,  description: the distance-dependent attenuation at high frequencies caused by the propagation medium
  //range: 0.892 to 1.0
  double flAirAbsorptionGainHF = 0.994;

  //AL_REVERB_ROOM_ROLLOFF_FACTOR,  description: attenuate the reflected sound,
  //range: 0.0 to 10.0
  double flRoomRolloffFactor = 0.0;

  //AL_REVERB_DECAY_HFLIMIT,  description: ,range:
  //int iDecayHFLimit;
};

struct EchoProperties
{
  //AL_ECHO_DELAY, description: delay between the original sound and the first ‘tap’, or echo instance
  //range: 0.0 to 0.207
  double flEchoDelay = 0.1;

  //AL_ECHO_LRDELAY, description: delay between the first ‘tap’ and the second ‘tap’.
  //range: 0.0 to 0.404
  double flEchoLRDelay = 0.1;

  //AL_ECHO_DAMPING, description: amount of high frequency damping applied to each echo
  //range: 0.0 to 0.99
  double flEchoDamping = 0.5;

  //AL_ECHO_FEEDBACK, description: amount of feedback the output signal fed back into the input
  //range: 0.0 to 1.0
  double flEchoFeedback = 0.5;

  //AL_ECHO_SPREAD, description: how hard panned the individual echoes are
  //range: -1.0 to 1.0
  double flEchoSpread = -1.0;
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
 * @brief event playing a track with echo effect
 */
struct AudioPlayEchoEvent
{
  SoundtrackID ID;
  EchoProperties echo_properties;
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
 * @brief event triggering a track with echo
 */
struct AudioTriggerEchoEvent
{
  AudioTrigger trigger;
  EchoProperties echo_properties;
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
 * @brief event playing a track with echo effect with 3D sound
 */
struct AudioPlayEcho3DEvent
{
  SoundtrackID ID;
  Coordinate3D position;
  EchoProperties echo_properties;
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
 * @brief event triggering a track with echo with 3D sound
 */
struct AudioTriggerEcho3DEvent
{
  AudioTrigger trigger;
  Coordinate3D position;
  EchoProperties echo_properties;
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
using AudioEvents =
    TypeList<AudioTriggerEvent, AudioTriggerEvent, AudioTrigger3DEvent, AudioTriggerReverbEvent, AudioTriggerEchoEvent,
             AudioTriggerReverb3DEvent, AudioTriggerEcho3DEvent, AudioPlayEvent, AudioPlayEchoEvent, AudioPlay3DEvent,
             AudioPlayReverbEvent, AudioPlayReverb3DEvent, AudioPlayEcho3DEvent, AudioMusicVolumeChangeEvent,
             AudioSoundVolumeChangeEvent, AudioSetMutedEvent, AudioStopEvent, AudioPruneEvent>;

#endif
