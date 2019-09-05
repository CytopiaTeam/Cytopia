#include "AudioMixer.hxx"
#include "basics/Settings.hxx"
#include "basics/LOG.hxx"

#include "../services/Randomizer.hxx"
#include "common/JsonSerialization.hxx"
#include <fstream>

using RuntimeError = std::runtime_error;
using ifstream = std::ifstream;
using nlohmann::json;

std::function<void(int)> AudioMixer::onTrackFinishedFunc;

AudioMixer::AudioMixer(GameService::ServiceTuple& context) : GameService(context)
{
  std::thread loadSoundThread(&AudioMixer::loadAllSounds, this);
  m_LoadSoundThread.swap(loadSoundThread);
}

AudioMixer::~AudioMixer()
{
  int num_opened = 0;
  int _discard;
  Uint16 _discard2;
  num_opened = Mix_QuerySpec(&_discard, &_discard2, &_discard);
  while(num_opened --> 0)
    Mix_CloseAudio();
  while(Mix_Init(0))
    Mix_Quit();
}

void AudioMixer::loadAllSounds()
{
  if(Mix_OpenAudio(44100, AUDIO_S16SYS, DEFAULT_CHANNELS::value, 1024) == -1)
    throw RuntimeError(string{"Unable to open audio channels "} + Mix_GetError());
  ifstream ifs {Settings::instance().audioConfigJSONFile.get()};
  json config_json;
  ifs >> config_json;
  AudioConfig audioConfig = config_json;

  /* Load all Music */
  loadSoundtrack(audioConfig.Music.begin(), audioConfig.Music.end(),
      [](const string& name, Mix_Chunk* chunk){
        return new Soundtrack{SoundtrackID{name}, ChannelID{0}, chunk, RepeatCount{0}, true, false, true, true};
      });

  /* Load all Sounds */
  loadSoundtrack(audioConfig.Sound.begin(), audioConfig.Sound.end(),
      [](const string& name, Mix_Chunk* chunk){
        return new Soundtrack{SoundtrackID{name}, ChannelID{-1}, chunk, RepeatCount{0}, false, false, true, true};
      });

  /* Generate m_GetSound mapping */
  std::for_each(m_Soundtracks.begin(), m_Soundtracks.end(), 
      [this](SoundtrackUPtr& ptr) {
        m_GetSound[ptr->ID] = &ptr;
      });

  /* Set up the Mix_ChannelFinished callback */
  onTrackFinishedFunc = [this](int channelID){ return onTrackFinished(channelID); };
  Mix_ChannelFinished(onTrackFinishedFuncPtr);
  LOG(LOG_INFO) << "soundloading complete";
  play(AudioTrigger::MainTheme);
}

void AudioMixer::joinLoadThread()
{
  if (m_LoadSoundThread.joinable())
  {
    LOG(LOG_INFO) << "joining soundloading thread";
    m_LoadSoundThread.join();
  }
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

void AudioMixer::play(SoundtrackID&& ID) noexcept { GetService<GameLoopMQ>().push(AudioPlayEvent{ID}); }

void AudioMixer::play(AudioTrigger&& trigger) noexcept 
{ 
  GetService<GameLoopMQ>().push(AudioTriggerEvent{trigger}); 
}

void AudioMixer::play(SoundtrackID&& ID, Coordinate3D&& position) noexcept 
{ 
  GetService<GameLoopMQ>().push(AudioPlay3DEvent{ID, position});
}

void AudioMixer::play(AudioTrigger&& trigger, Coordinate3D&& position) noexcept
{
  GetService<GameLoopMQ>().push(AudioTrigger3DEvent{trigger, position}); 
}

void AudioMixer::setMuted(bool isMuted) noexcept
{
  GetService<GameLoopMQ>().push(AudioSetMutedEvent{isMuted}); 
}

/*
+------------------+
|  Event Handlers  |
+------------------+
*/

void AudioMixer::handleEvent(const AudioTriggerEvent&& event)
{
  auto& possibilities = m_Triggers[event.trigger];
  if(possibilities.size() == 0) 
  {
    LOG() << "Warning: no Soundtracks are triggered by " << event.trigger._to_string();
    return;
  }
  SoundtrackID& trackID = *GetService<Randomizer>().choose(possibilities.begin(), possibilities.end());
  SoundtrackUPtr& track = *m_GetSound.at(trackID);
  playSoundtrack(track);
}

/* @todo Implement this */
void AudioMixer::handleEvent(const AudioTrigger3DEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}

void AudioMixer::handleEvent(const AudioPlayEvent&& event)
{
  SoundtrackUPtr& track = *m_GetSound.at(event.ID);
  playSoundtrack(track);
}

/* @todo Implement this */
void AudioMixer::handleEvent(const AudioPlay3DEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}

/* @todo Implement this */
void AudioMixer::handleEvent(const AudioSoundVolumeChangeEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}

/* @todo Implement this */
void AudioMixer::handleEvent(const AudioMusicVolumeChangeEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}

/* @todo Implement this */
void AudioMixer::handleEvent(const AudioSetMutedEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}

/*
+-----------+
|  Helpers  |
+-----------+
*/

void AudioMixer::playSoundtrack(SoundtrackUPtr& track)
{
  if(!track)
    throw RuntimeError("Received an invalid soundtrack");
  
  track->Channel = Mix_PlayChannel(track->Channel.get(), track->Chunks, 0);
  if(track->Channel.get() == -1)
  {
    /* We might need to allocate more channels */
    int num_alloc = Mix_AllocateChannels(-1);
    Mix_AllocateChannels(2 * num_alloc);
    track->Channel = Mix_PlayChannel(track->Channel.get(), track->Chunks, 0);
    if(track->Channel.get() == -1)
      throw RuntimeError("Failed to play track " + track->ID.get() + ": " + SDL_GetError());
  }
  m_Playing.push_front(&track);
  track->isPlaying = true;
}

template <typename Iterator, typename CallbackType>
void AudioMixer::loadSoundtrack(Iterator begin, Iterator end, CallbackType createSoundtrack)
{
  std::for_each(begin, end, 
    [this, &createSoundtrack](auto& kvp) {
      auto& [name, soundtrack] = kvp;
      string filepath = SDL_GetBasePath() + soundtrack.filePath;
      Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
      if(!chunk)
      {
        string ErrorMsg = "Error could not read sound file ";
        ErrorMsg += soundtrack.filePath;
        ErrorMsg += ": ";
        ErrorMsg += Mix_GetError();
        throw RuntimeError(ErrorMsg);
      }
      m_Soundtracks.emplace_back(createSoundtrack(name, chunk));
      for(AudioTrigger trigger : soundtrack.triggers)
        m_Triggers[trigger].emplace_back(name);
    });
}

void AudioMixer::onTrackFinished(int channelID)
{
  auto it = std::find_if(m_Playing.begin(), m_Playing.end(), 
    [channelID](const auto trackptr){
      const auto& track = *trackptr;
      return track && track->Channel.get() == channelID;
    });
  auto& track = **it;
  if(track)
  {
    track->isPlaying = false;
    if(!track->isMusic)
      track->Channel = -1;
  }
  m_Playing.erase(it);
}

