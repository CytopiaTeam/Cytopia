#include "AudioMixer.hxx"
#include "../engine/MessageQueue.hxx"
#include "basics/Settings.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "../services/ResourceManager.hxx"
#include "../services/GameClock.hxx"
#include "../services/Randomizer.hxx"
#include "common/JsonSerialization.hxx"
#include "Filesystem.hxx"

#include <fstream>

using ifstream = std::ifstream;
using nlohmann::json;

template <typename Type, size_t N> using Array = std::array<Type, N>;

std::function<void(int)> AudioMixer::onTrackFinishedFunc;

AudioMixer::AudioMixer()
{
  std::string jsonFileContent = fs::readFileAsString(Settings::instance().audioConfigJSONFile.get());
  json config_json = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (config_json.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().audioConfigJSONFile.get());

  AudioConfig audioConfig = config_json;
  for (auto &item : audioConfig.Music)
    for (auto &trigger : item.second.triggers)
      m_Triggers[trigger].emplace_back(item.first);
  for (auto &item : audioConfig.Sound)
    for (auto &trigger : item.second.triggers)
      m_Triggers[trigger].emplace_back(item.first);

  /* use default audio device */
  gAudioDevice = alcOpenDevice(nullptr);
  if (!gAudioDevice)
  {
    const char *error_msg = get_al_error_msg(alGetError());
    LOG(LOG_WARNING) << "Unable to initialize default audio device! " << error_msg;
    return;
  }

  /* create context */
  alContext = alcCreateContext(gAudioDevice, nullptr);
  if (!alContext)
  {
    const char *error_msg = get_al_error_msg(alGetError());
    throw AudioError(TRACE_INFO "Unable to initialize OpenAL context! " + error_msg);
  }
  alcMakeContextCurrent(alContext);

  /* Check if an error occurred, and clean up if so. */
  ALenum err;
  err = alGetError();
  if (err != AL_NO_ERROR)
    throw AudioError(TRACE_INFO "OpenAL error occurred: " + get_al_error_msg(err));

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
  GameClock::instance().addRealTimeClockTask(
      [&mixer = *this]
      {
        mixer.prune();
        return false;
      },
      0s, 5min);

  LOG(LOG_DEBUG) << "Created AudioMixer";
}

AudioMixer::~AudioMixer()
{
  if (!gAudioDevice)
  {
    return;
  }

  alcDestroyContext(alContext); //delete context
  alcCloseDevice(gAudioDevice); //close device

  LOG(LOG_DEBUG) << "Destroyed AudioMixer";
}

static SoundtrackUPtr noResoruce = nullptr;

SoundtrackUPtr &AudioMixer::getTrack(const AudioTrigger &trigger)
{
  if (!gAudioDevice)
  {
    return noResoruce;
  }

  auto &possibilities = m_Triggers[trigger];
  if (possibilities.empty())
  {
    LOG(LOG_WARNING) << "No Soundtracks are triggered by " << trigger._to_string();
    return noResoruce;
  }
  const SoundtrackID &trackID = *Randomizer::instance().choose(possibilities.begin(), possibilities.end());
  return ResourceManager::instance().get(trackID);
}

SoundtrackUPtr &AudioMixer::getTrack(const SoundtrackID &id)
{
  if (!gAudioDevice)
  {
    return noResoruce;
  }

  return ResourceManager::instance().get(id);
}

/*
   +---------------------+
   |  Service Interface  |
   +---------------------+
   */

void AudioMixer::setMusicVolume(float volume)
{
  for (auto it : m_Playing)
  {
    alSourcef((**it).source[1], AL_GAIN, volume);
  }
  // update the settings accordingly
  Settings::instance().musicVolume = volume;
}

void AudioMixer::setSoundEffectVolume(float volume)
{
  for (auto it : m_Playing)
  {
    alSourcef((**it).source[0], AL_GAIN, volume);
  }
  Settings::instance().soundEffectsVolume = volume;
}

void AudioMixer::play(const SoundtrackID &id, int effect)
{
  auto &track = getTrack(id);
  if (track)
  {
    if (effect == AL_EFFECT_NULL)
      playSoundtrack(track);
    else
      playSoundtrackWithEffect(track, effect);
  }
}

void AudioMixer::play(const AudioTrigger &trigger, int effect)
{
  auto &track = getTrack(trigger);
  if (track)
  {
    if (effect == AL_EFFECT_NULL)
      playSoundtrack(track);
    else
      playSoundtrackWithEffect(track, effect);
  }
}

void AudioMixer::play(const SoundtrackID &id, const Coordinate3D &position, int effect)
{
  if (getTrack(id))
  {
    auto &track = getTrack(id);
    /* set position of sources in track */
    alSource3f(track->source[0], AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y),
               static_cast<ALfloat>(position.z));
    alSource3f(track->source[1], AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y),
               static_cast<ALfloat>(position.z));
    play(id, effect);
  }
}

void AudioMixer::play(const AudioTrigger &trigger, const Coordinate3D &position, int effect)
{
  if (getTrack(trigger))
  {
    auto &track = getTrack(trigger);
    /* set position of sources in track
   * converted to regular Cartesian coordinate system */
    alSource3f(track->source[0], AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y),
               static_cast<ALfloat>(position.z));
    alSource3f(track->source[1], AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y),
               static_cast<ALfloat>(position.z));
    play(trigger, effect);
  }
}

void AudioMixer::setMuted(bool isMuted) { throw UnimplementedError(TRACE_INFO "Unimplemented Error"); }

void AudioMixer::stopAll()
{
  if (!gAudioDevice)
  {
    return;
  }

  while (!m_Playing.empty())
  {
    auto it = m_Playing.begin();
    alSourceStop((**it)->source[0]);
    alSourceStop((**it)->source[1]);
    (**it)->isPlaying = false;
    m_Playing.erase(it);
  }
}

void AudioMixer::prune()
{
  if (!gAudioDevice)
  {
    return;
  }

  for (auto it = m_Playing.begin(); it != m_Playing.end();)
  {
    int state = 0;
    alGetSourcei((**it)->source[(**it)->isMusic], AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING)
    {
      (**it)->isPlaying = false;
      it = m_Playing.erase(it);
    }
    else
    {
      ++it;
    }
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

  if (!track->source)
    throw AudioError{TRACE_INFO "Unable to play track because its source is uninitialized"};

  alSourcePlay(track->source[track->isMusic]);

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

void AudioMixer::playSoundtrackWithEffect(SoundtrackUPtr &track, int ALEffect)
{
  if (!track)
    throw AudioError(TRACE_INFO "Received an invalid soundtrack");

  if (!track->source)
    throw AudioError{TRACE_INFO "Unable to play track because its source is uninitialized"};

    //set up effect
#define LOAD_PROC(T, x) ((x) = (T)alGetProcAddress(#x))
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

  //load effect
  ALuint effect = 0;

  // Create the effect object
  alGenEffects(1, &effect);

  alEffecti(effect, AL_EFFECT_TYPE, ALEffect);

  // this is where we would put code to customize the properties of the effect

  // Check if an error occured, and clean up if so.
  ALenum err = alGetError();
  if (err != AL_NO_ERROR)
  {
    if (alIsEffect(effect))
      alDeleteEffects(1, &effect);

    throw AudioError{TRACE_INFO "Unable to add effect to track."};
    fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
  }

  /* Create the effect slot object. This is what "plays" an effect on sources
   * that connect to it. */
  alGenAuxiliaryEffectSlots(1, &track->effect_slot);

  /* Tell the effect slot to use the loaded effect object. Note that this
   * effectively copies the effect properties. You can modify or delete the
   * effect object afterward without affecting the effect slot.
   */
  alAuxiliaryEffectSloti(track->effect_slot, AL_EFFECTSLOT_EFFECT, (ALint)effect);
  assert(alGetError() == AL_NO_ERROR && "Failed to set effect slot");

  alDeleteEffects(1, &effect);

  //apply effect to source
  alSource3i(track->source[track->isMusic], AL_AUXILIARY_SEND_FILTER, (ALint)(track->effect_slot), 0, AL_FILTER_NULL);
  assert(alGetError() == AL_NO_ERROR && "Failed to setup reverb for sound source send 0.");

  //play sound
  alSourcePlay(track->source[track->isMusic]);
  m_Playing.push_front(&track);
  track->isPlaying = true;
}

void AudioMixer::onTrackFinished(int channelID)
{
  auto it = std::find_if(m_Playing.begin(), m_Playing.end(),
                         [channelID](const auto trackptr)
                         {
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

const char *AudioMixer::get_al_error_msg(ALenum error)
{
  switch (error)
  {
  case AL_INVALID_NAME:
    return "a bad name (ID) was passed to an OpenAL function";
  case AL_INVALID_ENUM:
    return "an invalid enum value was passed to an OpenAL function";
  case AL_INVALID_VALUE:
    return "an invalid value was passed to an OpenAL function";
  case AL_INVALID_OPERATION:
    return "the requested operation is not valid";
  case AL_OUT_OF_MEMORY:
    return "the requested operation resulted in OpenAL running out of memory";
  case AL_NO_ERROR:
    return "there is not currently an error";
  default:
    return "Unknown error";
  }
}
