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
  #ifdef USE_OPENAL_SOFT
  alcDestroyContext(alContext);	//delete context
  alcCloseDevice(gAudioDevice);	//close device
  #endif
    
}

void AudioMixer::loadAllSounds()
{
  #ifdef USE_OPENAL_SOFT
  if(Settings::instance().audio3DStatus)
  {
	  //LOG(LOG_DEBUG) << "3D audio on! \n";
	  if(Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 1024) == -1)
		throw RuntimeError(string{"Unable to open audio channels "} + Mix_GetError());
  }
  else
  {
	  //LOG(LOG_DEBUG) << "3D audio off! \n";
	  if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) == -1)
		throw RuntimeError(string{"Unable to open audio channels "} + Mix_GetError());
  }
  
  #else
   if(Mix_OpenAudio(44100, AUDIO_S16SYS, DEFAULT_CHANNELS::value, 1024) == -1)
    throw RuntimeError(string{"Unable to open audio channels "} + Mix_GetError());
  #endif
  
  ifstream ifs;

  LOG(LOG_INFO) << "Loaded from audio Json config.\n";
  ifs.open(SDL_GetBasePath() + Settings::instance().audioConfigJSONFile.get());
  
  json config_json;
  ifs >> config_json;
  AudioConfig audioConfig = config_json;
  
   #ifdef USE_OPENAL_SOFT
	//use default audio device
	gAudioDevice = alcOpenDevice(nullptr);
	if(!gAudioDevice)
	{
		throw RuntimeError(string{"Unable to initialize default audio device! " + *alGetString(alGetError()) } );
	}

	//create context
	alContext = alcCreateContext(gAudioDevice, nullptr);
	if(!alContext)
	{
		throw RuntimeError(string{"Unable to initialize OpenAL context! " + *alGetString(alGetError()) } );
	}
	else{alcMakeContextCurrent(alContext);}

	/* Check if an error occured, and clean up if so. */
	ALenum err; //variable for error flag
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		throw RuntimeError(string{ alGetString(err) } );
	}
	
	LOG(LOG_INFO) << " Open AL Soft Initialized! \n";
	
	//set listener position one space behind origin
	std::array <float, 3> listener_position_vector = { 0.0f, 0.0f, 1.0f };
	
	//Initialize Listener speed
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);//is not moving in 3d space
	
	//initialize listener position 1 space behind origin in openal forward direction(-z direction in regular cartesian) 
	alListener3f(AL_POSITION, listener_position_vector[static_cast<int>(AudioMixer::POSITION_INDEX::X)], listener_position_vector[static_cast<int>(AudioMixer::POSITION_INDEX::Y)], listener_position_vector[static_cast<int>(AudioMixer::POSITION_INDEX::Z)]);
	
	//Set Listener orientation
	
	//set to just facing the screen

	//set where head is facing
	//forward x, forward y, forward z, up x, up y, up z
	std::array <float, 6> listener_orientation_vector = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
		
	//set current listener orientation
	alListenerfv(AL_ORIENTATION, listener_orientation_vector.data());
	
	
#endif

  //LLOG(LOG_DEBUG) << "Loading all sounds. \n";
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
  //LOG(LOG_DEBUG) << "Finished loading all sounds.\n";
}

void AudioMixer::joinLoadThread()
{
  running = false;
  if (m_LoadSoundThread.joinable())
  {
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

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(SoundtrackID&& ID, Coordinate3D&& position) noexcept 
{ 
  GetService<GameLoopMQ>().push(AudioPlay3DEvent{ID, position});
}
#endif

#ifdef USE_OPENAL_SOFT
void AudioMixer::play(AudioTrigger&& trigger, Coordinate3D&& position) noexcept
{
  GetService<GameLoopMQ>().push(AudioTrigger3DEvent{trigger, position}); 
}
#endif

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

#ifdef USE_OPENAL_SOFT
void AudioMixer::handleEvent(const AudioTrigger3DEvent&& event)
{
  #ifdef USE_OPENAL_SOFT
  auto& possibilities = m_Triggers[event.trigger];
  if(possibilities.size() == 0) 
  {
    LOG() << "Warning: no Soundtracks are triggered by " << event.trigger._to_string();
    return;
  }
  SoundtrackID& trackID = *GetService<Randomizer>().choose(possibilities.begin(), possibilities.end());
  SoundtrackUPtr& track = *m_GetSound.at(trackID);
  
   //set position of source in track 
   //converted to regular cartesian coordinate system
  alSource3f(track->source, AL_POSITION, 
		static_cast<ALfloat>(event.position.x), 
		static_cast<ALfloat>(event.position.y), 
		static_cast<ALfloat>(event.position.z));
  playSoundtrack(track);
  #endif
}
#endif

void AudioMixer::handleEvent(const AudioPlayEvent&& event)
{
  SoundtrackUPtr& track = *m_GetSound.at(event.ID);
  playSoundtrack(track);
}

#ifdef USE_OPENAL_SOFT
void AudioMixer::handleEvent(const AudioPlay3DEvent&& event)
{
  SoundtrackUPtr& track = *m_GetSound.at(event.ID);
   //set position of source in track 
  alSource3f(track->source, AL_POSITION, 
		static_cast<ALfloat>(event.position.x), 
		static_cast<ALfloat>(event.position.y), 
		static_cast<ALfloat>(event.position.z));
  playSoundtrack(track);
}
#endif

void AudioMixer::handleEvent(const AudioSoundVolumeChangeEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}


void AudioMixer::handleEvent(const AudioMusicVolumeChangeEvent&& event)
{
  string ErrorMsg = "Unimplemented Error: ";
  ErrorMsg += __PRETTY_FUNCTION__;
  throw RuntimeError(ErrorMsg);
}


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
  
  #ifndef USE_OPENAL_SOFT
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

  #else
  //LOG(LOG_DEBUG) << "OpenAL Soft called to play track! \n";
  if(track->source != 0)
  {
	  alSourcePlay(track->source);
	  m_Playing.push_front(&track);
	  track->isPlaying = true;
  }
  else
  {
	  throw RuntimeError{"Unable to play track because it's source is uninitialized! \n"};
  }
  
  #endif
  
}



template <typename Iterator, typename CallbackType>
void AudioMixer::loadSoundtrack(Iterator begin, Iterator end, CallbackType createSoundtrack)
{
  std::for_each(begin, end, 
    [this, &createSoundtrack](auto& kvp) {
      if (running)
      {
		auto& [name, soundtrack] = kvp;
		string filepath;
		
		#ifdef USE_OPENAL_SOFT
		if(Settings::instance().audio3DStatus)
		{
			filepath = SDL_GetBasePath() + soundtrack.monoFilePath;
		}
		else
		{
			filepath = SDL_GetBasePath() + soundtrack.stereoFilePath; 
		}
        #else
        filepath = SDL_GetBasePath() + soundtrack.stereoFilePath; 
        #endif
        Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
        if(!chunk)
        {
          string ErrorMsg = "Error could not read sound file ";
          ErrorMsg += filepath;
          ErrorMsg += ": ";
          ErrorMsg += Mix_GetError();
          throw RuntimeError(ErrorMsg);
        }
        m_Soundtracks.emplace_back(createSoundtrack(name, chunk));
        
        #ifdef USE_OPENAL_SOFT
      
      //initialize buffer
	  alGenBuffers(1, &m_Soundtracks.back()->buffer);
	  ALenum errorCode = alGetError();
	  if(errorCode != AL_NO_ERROR)
	    throw RuntimeError("Could not create buffers: Error " + std::to_string(errorCode));
	    
      //set buffer data
	 
	  ALenum format;
	  if(Settings::instance().audio3DStatus){format = AL_FORMAT_MONO16;}
	  else{format = AL_FORMAT_STEREO16;}
	  // parameters buffer, format, data, sample length, frequency(sample rate)
	  alBufferData(m_Soundtracks.back()->buffer, format, m_Soundtracks.back()->Chunks->abuf, m_Soundtracks.back()->Chunks->alen, 44100);
      
      //initialize source
      alGenSources(1, &m_Soundtracks.back()->source);
	  alSourcei(m_Soundtracks.back()->source, AL_SOURCE_RELATIVE, AL_FALSE);
	  errorCode = alGetError();
      if(errorCode != AL_NO_ERROR)
		throw RuntimeError("Failed to setup sound source: Error " + std::to_string(errorCode));
		
      //attach buffer to source
      alSourcei(m_Soundtracks.back()->source, AL_BUFFER, m_Soundtracks.back()->buffer);
      
      //free chunk
      Mix_FreeChunk(m_Soundtracks.back()->Chunks);
      m_Soundtracks.back()->Chunks = nullptr;
      
      #endif
      
      for(AudioTrigger trigger : soundtrack.triggers)
	    m_Triggers[trigger].emplace_back(name);
        
      }
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

