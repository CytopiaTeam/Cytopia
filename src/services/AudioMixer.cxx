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

//for orientation of listener
enum ORIENTATION_INDEX { FORWARD_X=0,FORWARD_Y=1,FORWARD_Z=2,
													 UP_X=3, UP_Y=4, UP_Z=5 };

//for position of listener
enum POSITION_INDEX { X=0,Y=1,Z=2 };

AudioMixer::AudioMixer(GameService::ServiceTuple& context) : GameService(context)
{
  #ifdef USE_OPENAL_SOFT
  if(Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 1024) == -1)
    throw RuntimeError(string{"Unable to open audio channels "} + Mix_GetError());
  #else
   if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) == -1)
    throw RuntimeError(string{"Unable to open audio channels "} + Mix_GetError());
  #endif
  
  // #ifdef USE_OPENAL_SOFT
  // ifstream ifs {Settings::instance().audioConfig3DJSONFile.get()};
  //  #else
  //ifstream ifs {Settings::instance().audioConfigJSONFile.get()};
  //#endif
  
  ifstream ifs {Settings::instance().audioConfigJSONFile.get()};
  json config_json;
  ifs >> config_json;
  AudioConfig audioConfig = config_json;
  
   #ifdef USE_OPENAL_SOFT
	//use default audio device
	gAudioDevice = alcOpenDevice(NULL);
	if(gAudioDevice == NULL)
	{
		throw RuntimeError(string{"Unable to initialize default audio device! " + *alGetString(alGetError()) } );
	}

	//create context
	alContext = alcCreateContext(gAudioDevice, NULL);
	if(alContext == NULL)
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
	
	std::cout << " Open AL Soft Initialized! \n";
	
	std::vector <float> listener_position_vector(3);
	//set listener position one space behind origin
	listener_position_vector[POSITION_INDEX::X] = 0.0f;
	listener_position_vector[POSITION_INDEX::Y] = 0.0f;
	listener_position_vector[POSITION_INDEX::Z] = 1.0f;
	
	//Initialize Listener speed
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);//is not moving in 3d space
	
	//initialize listener position 1 space behind origin in openal forward direction(-z direction in regular cartesian) 
	alListener3f(AL_POSITION, listener_position_vector[POSITION_INDEX::X], listener_position_vector[POSITION_INDEX::Y], listener_position_vector[POSITION_INDEX::Z]);
	
	//Set Listener orientation
	
	//set to just facing the screen

	//set where head is facing
	std::vector <float> listener_orientation_vector(6);
	listener_orientation_vector[ORIENTATION_INDEX::FORWARD_X] = 0.0f; //forward vector x value
	listener_orientation_vector[ORIENTATION_INDEX::FORWARD_Y] = 0.0f; //forward vector y value
	listener_orientation_vector[ORIENTATION_INDEX::FORWARD_Z] = -1.0f; //forward vector z value
	//set direction of top of head surface vector
	listener_orientation_vector[ORIENTATION_INDEX::UP_X] = 0.0f; //up vector x value
	listener_orientation_vector[ORIENTATION_INDEX::UP_Y] = 1.0f; //up vector y value
	listener_orientation_vector[ORIENTATION_INDEX::UP_Z] = 0.0f; //up vector z value
		
	//set current listener orientation
	alListenerfv(AL_ORIENTATION, listener_orientation_vector.data());
	
	
#endif

    
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
		(ALfloat)event.position.x, 
		(ALfloat)event.position.y, 
		(ALfloat)event.position.z);
  playSoundtrack(track);
  #endif
}

void AudioMixer::handleEvent(const AudioPlayEvent&& event)
{
  SoundtrackUPtr& track = *m_GetSound.at(event.ID);
  playSoundtrack(track);
}

/* @todo Implement this */
void AudioMixer::handleEvent(const AudioPlay3DEvent&& event)
{
  SoundtrackUPtr& track = *m_GetSound.at(event.ID);
  #ifdef USE_OPENAL_SOFT
   //set position of source in track 
  alSource3f(track->source, AL_POSITION, 
		(ALfloat)event.position.z, 
		(ALfloat)event.position.x, 
		(ALfloat)event.position.y);
  playSoundtrack(track);
  #endif
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
  std::cout << "OpenAL Soft called to play track! \n";
  if(track->source != 0)
  {
	  alSourcePlay(track->source);
	  m_Playing.push_front(&track);
	  track->isPlaying = true;
  }
  else
  {
	  std::cout << "Unable to play track because it's source is uninitialized! \n";
  }
  
  #endif
  
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
      
      #ifdef USE_OPENAL_SOFT
      
      //initialize buffer
	  alGenBuffers(1, &m_Soundtracks.back()->buffer);
	  assert(alGetError() == AL_NO_ERROR && "Could not create buffers");
      //set buffer data
	  //alBufferData(buffer, format, data, slen, frequency);
	  ALenum format;
	  //if(){format = AL_FORMAT_MONO16;}
	  //else if(){format = AL_FORMAT_STEREO16;}
	  alBufferData(m_Soundtracks.back()->buffer, AL_FORMAT_MONO16, m_Soundtracks.back()->Chunks->abuf, m_Soundtracks.back()->Chunks->alen, 44100);
      
      //initialize source
      alGenSources(1, &m_Soundtracks.back()->source);
	  alSourcei(m_Soundtracks.back()->source, AL_SOURCE_RELATIVE, AL_FALSE);
	  assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source.");
      
      //attach buffer to source
      alSourcei(m_Soundtracks.back()->source, AL_BUFFER, m_Soundtracks.back()->buffer);
      
      //free chunk
      Mix_FreeChunk(m_Soundtracks.back()->Chunks);
      m_Soundtracks.back()->Chunks = nullptr;
      
      #endif
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

