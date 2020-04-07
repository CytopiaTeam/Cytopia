#include "AudioMixer.hxx"
#include "../engine/MessageQueue.hxx"
#include "basics/Settings.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "../services/ResourceManager.hxx"
#include "../services/GameClock.hxx"
#include "../services/Randomizer.hxx"
#include "common/JsonSerialization.hxx"

#include <fstream>

using ifstream = std::ifstream;
using nlohmann::json;

template <typename Type, size_t N> using Array = std::array<Type, N>;

std::function<void(int)> AudioMixer::onTrackFinishedFunc;

AudioMixer::AudioMixer(GameService::ServiceTuple &context) : GameService(context)
{
  string fName = SDL_GetBasePath() + Settings::instance().audioConfigJSONFile.get();
  ifstream ifs(fName);
  if (!ifs)
    throw ConfigurationError(TRACE_INFO "Couldn't open file " + fName);
  json config_json;
  ifs >> config_json;
  AudioConfig audioConfig = config_json;
  for(auto & item : audioConfig.Music)
    for (auto & trigger : item.second.triggers)
      m_Triggers[trigger].emplace_back(item.first);
  for (auto &item : audioConfig.Sound)
    for (auto &trigger : item.second.triggers)
      m_Triggers[trigger].emplace_back(item.first);
#ifdef USE_OPENAL_SOFT
  if (Settings::instance().audio3DStatus)
  {
    LOG(LOG_INFO) << "Using mono sounds";
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 1024) == -1)
      throw AudioError(TRACE_INFO + string{"Unable to open audio channels "} + Mix_GetError());
  }
  else
  {
    LOG(LOG_INFO) << "Using stereo sounds";
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) == -1)
      throw AudioError(TRACE_INFO + string{"Unable to open audio channels "} + Mix_GetError());
  }
  /* use default audio device */
  gAudioDevice = alcOpenDevice(nullptr);
  if (!gAudioDevice)
    throw AudioError(TRACE_INFO "Unable to initialize default audio device! " + *alGetString(alGetError()));

  /* create context */
  alContext = alcCreateContext(gAudioDevice, nullptr);
  if (!alContext)
    throw AudioError(TRACE_INFO "Unable to initialize OpenAL context! " + *alGetString(alGetError()));
  else
    alcMakeContextCurrent(alContext);

  /* Check if an error occured, and clean up if so. */
  ALenum err;
  err = alGetError();
  if (err != AL_NO_ERROR)
    throw AudioError(TRACE_INFO "OpenAL error occured: " + alGetString(err));

  /* set listener position one space behind origin */
  Array<float, 3> listener_position_vector{0.0f, 0.0f, 1.0f};

  /* Initialize Listener speed */
  alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f); // is not moving in 3d space

  /* initialize listener position 1 space behind origin in openal
     * forward direction(-z direction in regular cartesian) */
  alListener3f(AL_POSITION, listener_position_vector[static_cast<int>(AudioMixer::POSITION_INDEX::X)],
               listener_position_vector[static_cast<int>(AudioMixer::POSITION_INDEX::Y)],
               listener_position_vector[static_cast<int>(AudioMixer::POSITION_INDEX::Z)]);

  /* Set Listener orientation
     * forward x, forward y, forward z, up x, up y, up z */
  Array<float, 6> listener_orientation_vector{0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};
  alListenerfv(AL_ORIENTATION, listener_orientation_vector.data());
  

  /* Set a pruning repeated task to get rid of soundtracks that have finished playing */
  GetService<GameClock>().createRepeatedTask(5min, [&mixer = *this]() { mixer.prune(); });

#else  // USE_OPENAL_SOFT
  LOG(LOG_INFO) << "Using stereo sounds";
  if (Mix_OpenAudio(44100, AUDIO_S16SYS, DEFAULT_CHANNELS::value, 1024) == -1)
    throw AudioError(TRACE_INFO + string{"Unable to open audio channels "} + Mix_GetError());
  /* Set up the Mix_ChannelFinished callback */
  onTrackFinishedFunc = [this](int channelID) { return onTrackFinished(channelID); };
  Mix_ChannelFinished(onTrackFinishedFuncPtr);
#endif // USE_OPENAL_SOFT
  LOG(LOG_DEBUG) << "Created AudioMixer";
}

AudioMixer::~AudioMixer()
{
  int num_opened = 0;
  int _discard;
  Uint16 _discard2;
  num_opened = Mix_QuerySpec(&_discard, &_discard2, &_discard);
  while (num_opened-- > 0)
    Mix_CloseAudio();
  while (Mix_Init(0))
    Mix_Quit();
#ifdef USE_OPENAL_SOFT
  alcDestroyContext(alContext); //delete context
  alcCloseDevice(gAudioDevice); //close device
#endif
  LOG(LOG_DEBUG) << "Destroyed AudioMixer";
}

/*
   +---------------------+
   |  Service Interface  |
   +---------------------+
   */

void AudioMixer::setMusicVolume(VolumeLevel volume) noexcept
{
  GetService<GameLoopMQ>().push(AudioMusicVolumeChangeEvent{volume});
}

void AudioMixer::setSoundEffectVolume(VolumeLevel volume) noexcept
{
  GetService<GameLoopMQ>().push(AudioSoundVolumeChangeEvent{volume});
}

void AudioMixer::play(SoundtrackID &&ID) noexcept { GetService<GameLoopMQ>().push(AudioPlayEvent{ID}); }

void AudioMixer::play(AudioTrigger &&trigger) noexcept { GetService<GameLoopMQ>().push(AudioTriggerEvent{trigger}); }

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(SoundtrackID &&ID, Coordinate3D &&position) noexcept
{
  GetService<GameLoopMQ>().push(AudioPlay3DEvent{ID, position});
}
#endif

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(AudioTrigger &&trigger, Coordinate3D &&position) noexcept
{
  GetService<GameLoopMQ>().push(AudioTrigger3DEvent{trigger, position});
}

#endif

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(SoundtrackID &&ID, StandardReverbProperties &&reverb_properties) noexcept
{
  GetService<GameLoopMQ>().push(AudioPlayReverbEvent{ID, reverb_properties});
}
#endif

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(AudioTrigger &&trigger, StandardReverbProperties &&reverb_properties) noexcept
{
  GetService<GameLoopMQ>().push(AudioTriggerReverbEvent{trigger, reverb_properties});
}
#endif

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(SoundtrackID &&ID, Coordinate3D &&position, StandardReverbProperties &&reverb_properties) noexcept
{
  GetService<GameLoopMQ>().push(AudioPlayReverb3DEvent{ID, position, reverb_properties});
}
#endif

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(AudioTrigger &&trigger, Coordinate3D &&position, StandardReverbProperties &&reverb_properties) noexcept
{
  GetService<GameLoopMQ>().push(AudioTriggerReverb3DEvent{trigger, position,reverb_properties});
}
#endif

void AudioMixer::setMuted(bool isMuted) noexcept { GetService<GameLoopMQ>().push(AudioSetMutedEvent{isMuted}); }

void AudioMixer::stopAll() noexcept { GetService<GameLoopMQ>().push(AudioStopEvent{}); }

void AudioMixer::prune() noexcept { GetService<GameLoopMQ>().push(AudioPruneEvent{}); }

/*
   +------------------+
   |  Event Handlers  |
   +------------------+
   */

void AudioMixer::handleEvent(const AudioTriggerEvent &&event)
{
  auto &possibilities = m_Triggers[event.trigger];
  if (possibilities.size() == 0)
  {
    LOG(LOG_WARNING) << "No Soundtracks are triggered by " << event.trigger._to_string();
    return;
  }
  SoundtrackID &trackID = *GetService<Randomizer>().choose(possibilities.begin(), possibilities.end());
  SoundtrackUPtr & track = GetService<ResourceManager>().get(trackID);
  playSoundtrack(track);
}

void AudioMixer::handleEvent(const AudioPlayEvent &&event)
{
  SoundtrackUPtr & track = GetService<ResourceManager>().get(event.ID);
  playSoundtrack(track);
}

#ifdef USE_OPENAL_SOFT

void AudioMixer::handleEvent(const AudioTrigger3DEvent &&event)
{
  auto &possibilities = m_Triggers[event.trigger];
  if (possibilities.size() == 0)
  {
    LOG(LOG_WARNING) << "Warning: no Soundtracks are triggered by " << event.trigger._to_string();
    return;
  }
  SoundtrackID &trackID = *GetService<Randomizer>().choose(possibilities.begin(), possibilities.end());
  SoundtrackUPtr & track = GetService<ResourceManager>().get(trackID);

  /* set position of source in track 
   * converted to regular cartesian coordinate system */
  alSource3f(track->source, AL_POSITION, static_cast<ALfloat>(event.position.x), static_cast<ALfloat>(event.position.y),
             static_cast<ALfloat>(event.position.z));
  playSoundtrack(track);
}

void AudioMixer::handleEvent(const AudioPlay3DEvent &&event)
{
  SoundtrackUPtr & track = GetService<ResourceManager>().get(event.ID);
  /* set position of source in track */
  alSource3f(track->source, AL_POSITION, static_cast<ALfloat>(event.position.x), static_cast<ALfloat>(event.position.y),
             static_cast<ALfloat>(event.position.z));
  playSoundtrack(track);
}

void AudioMixer::handleEvent(const AudioTriggerReverbEvent &&event)
{
  auto &possibilities = m_Triggers[event.trigger];
  if (possibilities.size() == 0)
  {
    LOG(LOG_WARNING) << "Warning: no Soundtracks are triggered by " << event.trigger._to_string();
    return;
  }
  SoundtrackID &trackID = *GetService<Randomizer>().choose(possibilities.begin(), possibilities.end());
  SoundtrackUPtr & track = GetService<ResourceManager>().get(trackID);

  playSoundtrackWithReverb(track, event.reverb_properties);
}

void AudioMixer::handleEvent(const AudioPlayReverbEvent &&event)
{
  SoundtrackUPtr & track = GetService<ResourceManager>().get(event.ID);

  playSoundtrackWithReverb(track,event.reverb_properties);
}

void AudioMixer::handleEvent(const AudioTriggerReverb3DEvent &&event)
{
  auto &possibilities = m_Triggers[event.trigger];
  if (possibilities.size() == 0)
  {
    LOG(LOG_WARNING) << "Warning: no Soundtracks are triggered by " << event.trigger._to_string();
    return;
  }
  SoundtrackID &trackID = *GetService<Randomizer>().choose(possibilities.begin(), possibilities.end());
  SoundtrackUPtr & track = GetService<ResourceManager>().get(trackID);

  /* set position of source in track 
   * converted to regular cartesian coordinate system */
  alSource3f(track->source, AL_POSITION, static_cast<ALfloat>(event.position.x), static_cast<ALfloat>(event.position.y),
             static_cast<ALfloat>(event.position.z));
  playSoundtrackWithReverb(track, event.reverb_properties);
}

void AudioMixer::handleEvent(const AudioPlayReverb3DEvent &&event)
{
  SoundtrackUPtr & track = GetService<ResourceManager>().get(event.ID);
  /* set position of source in track */
  alSource3f(track->source, AL_POSITION, static_cast<ALfloat>(event.position.x), static_cast<ALfloat>(event.position.y),
             static_cast<ALfloat>(event.position.z));
  playSoundtrackWithReverb(track,event.reverb_properties);
}

#endif // USE_OPENAL_SOFT

void AudioMixer::handleEvent(const AudioSoundVolumeChangeEvent &&event)
{
  throw UnimplementedError(TRACE_INFO "Unimplemented Error");
}

void AudioMixer::handleEvent(const AudioMusicVolumeChangeEvent &&event)
{
  throw UnimplementedError(TRACE_INFO "Unimplemented Error");
}

void AudioMixer::handleEvent(const AudioSetMutedEvent &&event) { throw UnimplementedError(TRACE_INFO "Unimplemented Error"); }

void AudioMixer::handleEvent(const AudioStopEvent &&)
{
	#ifndef USE_OPENAL_SOFT
	Mix_HaltChannel(-1);
	#else // USE_OPENAL_SOFT
  while(!m_Playing.empty())
  {
    auto it = m_Playing.begin();
    alSourceStop((**it)->source);
    (**it)->isPlaying = false;
    m_Playing.erase(it);
	}
	#endif // USE_OPENAL_SOFT
}

void AudioMixer::handleEvent(const AudioPruneEvent &&)
{
	for(auto it = m_Playing.begin(); it != m_Playing.end();)
	{
		int state = 0;
		alGetSourcei((**it)->source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
		{
			(**it)->isPlaying = false;
			it = m_Playing.erase(it);
		}
		else { ++it; }
	}
}

/*
   +-----------+
   |  Helpers  |
   +-----------+
   */

void AudioMixer::playSoundtrack(SoundtrackUPtr &track)
{
  if (!track)
    throw AudioError(TRACE_INFO "Received an invalid soundtrack");

#ifndef USE_OPENAL_SOFT
  track->Channel = Mix_PlayChannel(track->Channel.get(), track->Chunks, 0);
  if (track->Channel.get() == -1)
  {
    /* We might need to allocate more channels */
    int num_alloc = Mix_AllocateChannels(-1);
    Mix_AllocateChannels(2 * num_alloc);
    track->Channel = Mix_PlayChannel(track->Channel.get(), track->Chunks, 0);
    if (track->Channel.get() == -1)
      throw AudioError(TRACE_INFO "Failed to play track " + track->ID.get() + ": " + SDL_GetError());
  }
#else // USE_OPENAL_SOFT
  if (!track->source)
    throw AudioError{TRACE_INFO "Unable to play track because its source is uninitialized"};
  alSourcePlay(track->source);
#endif // USE_OPENAL_SOFT
  m_Playing.push_front(&track);
  track->isPlaying = true;
}


/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;


void AudioMixer::playSoundtrackWithReverb(SoundtrackUPtr &track,StandardReverbProperties reverb_properties)
{
	if (!track)
    throw AudioError(TRACE_INFO "Received an invalid soundtrack");

#ifndef USE_OPENAL_SOFT
  track->Channel = Mix_PlayChannel(track->Channel.get(), track->Chunks, 0);
  if (track->Channel.get() == -1)
  {
    /* We might need to allocate more channels */
    int num_alloc = Mix_AllocateChannels(-1);
    Mix_AllocateChannels(2 * num_alloc);
    track->Channel = Mix_PlayChannel(track->Channel.get(), track->Chunks, 0);
    if (track->Channel.get() == -1)
      throw AudioError(TRACE_INFO "Failed to play track " + track->ID.get() + ": " + SDL_GetError());
  }
#else // USE_OPENAL_SOFT
  if (!track->source)
    throw AudioError{TRACE_INFO "Unable to play track because its source is uninitialized"};
  
//set up effect

    #define LOAD_PROC(T, x)  ((x) = (T)alGetProcAddress(#x))
		LOAD_PROC(LPALGENEFFECTS, alGenEffects);
		LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
		LOAD_PROC(LPALISEFFECT, alIsEffect);
		LOAD_PROC(LPALEFFECTI, alEffecti);
		LOAD_PROC(LPALEFFECTIV, alEffectiv);
		LOAD_PROC(LPALEFFECTF, alEffectf);
		LOAD_PROC(LPALEFFECTFV, alEffectfv);
		LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
		LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
		LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
		LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

		LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
		LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
		LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
	#undef LOAD_PROC
  
  //initialize reverb property
  EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_GENERIC;

  //assign custom properties
  reverb.flDensity = reverb_properties.flDensity;
  reverb.flDiffusion = reverb_properties.flDiffusion;
  reverb.flGain = reverb_properties.flGain;
  reverb.flGainHF = reverb_properties.flGainHF;
  reverb.flDecayTime = reverb_properties.flDecayTime;
  reverb.flDecayHFRatio = reverb_properties.flDecayHFRatio;
  reverb.flReflectionsGain = reverb_properties.flReflectionsGain;
  reverb.flReflectionsDelay = reverb_properties.flReflectionsDelay;
  reverb.flLateReverbGain = reverb_properties.flLateReverbGain;
  reverb.flLateReverbDelay = reverb_properties.flLateReverbDelay;
  reverb.flAirAbsorptionGainHF = reverb_properties.flAirAbsorptionGainHF;
  reverb.flRoomRolloffFactor = reverb_properties.flRoomRolloffFactor;

  //load effect
  ALuint effect = 0;
  
  alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

  alEffectf(effect, AL_REVERB_DENSITY, reverb.flDensity);
  alEffectf(effect, AL_REVERB_DIFFUSION, reverb.flDiffusion);
  alEffectf(effect, AL_REVERB_GAIN, reverb.flGain);
  alEffectf(effect, AL_REVERB_GAINHF, reverb.flGainHF);
  alEffectf(effect, AL_REVERB_DECAY_TIME, reverb.flDecayTime);
  alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb.flDecayHFRatio);
  alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb.flReflectionsGain);
  alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb.flReflectionsDelay);
  alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb.flLateReverbGain);
  alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb.flLateReverbDelay);
  alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb.flAirAbsorptionGainHF);
  alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.flRoomRolloffFactor);
  alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb.iDecayHFLimit);
  
	/* Check if an error occured, and clean up if so. */
	ALenum err = alGetError();
	if(err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
		if(alIsEffect(effect))
			alDeleteEffects(1, &effect);
		return;
	}

  /* Create the effect slot object. This is what "plays" an effect on sources
   * that connect to it. */
  alGenAuxiliaryEffectSlots(1, &track->effect_slot);
  
  /* Tell the effect slot to use the loaded effect object. Note that the this
     * effectively copies the effect properties. You can modify or delete the
     * effect object afterward without affecting the effect slot.
     */
  alAuxiliaryEffectSloti(track->effect_slot, AL_EFFECTSLOT_EFFECT, (ALint)effect);
  assert(alGetError()== AL_NO_ERROR && "Failed to set effect slot");
  
  alDeleteEffects(1, &effect);
  
  //apply effect to source
  alSource3i(track->source, AL_AUXILIARY_SEND_FILTER, (ALint)(track->effect_slot), 0, AL_FILTER_NULL);
  assert(alGetError()== AL_NO_ERROR && "Failed to setup reverb for sound source send 0.");
  
//play sound  
    
  alSourcePlay(track->source);
  
#endif // USE_OPENAL_SOFT
  m_Playing.push_front(&track);
  track->isPlaying = true;
  
}


void AudioMixer::onTrackFinished(int channelID)
{
  auto it = std::find_if(m_Playing.begin(), m_Playing.end(), [channelID](const auto trackptr) {
    const auto &track = *trackptr;
    return track && track->Channel.get() == channelID;
  });
  auto &track = **it;
  if (track)
  {
    track->isPlaying = false;
    if (!track->isMusic)
      track->Channel = -1;
  }
  m_Playing.erase(it);
}
