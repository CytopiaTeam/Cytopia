#include "AudioMixer.hxx"

#include "basics/Settings.hxx"

AudioMixer::AudioMixer()
{
  Mix_OpenAudio(44100, AUDIO_S16SYS, Settings::instance().audioChannels, 1024);
  setMusicVolume(Settings::instance().musicVolume);
  
  #ifdef USE_OPENAL_SOFT
	//use default audio device
	gAudioDevice = alcOpenDevice(NULL);
	if(gAudioDevice == NULL)
	{
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "Unable to initialize default audio device! \n";
		//return false;
	}

	//create context
	alContext = alcCreateContext(gAudioDevice, NULL);
	if(alContext == NULL)
	{
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "Unable to initialize OpenAL context! \n";
		//return false;
	}
	else{alcMakeContextCurrent(alContext);}

	/* Check if an error occured, and clean up if so. */
	ALenum err; //variable for error flag
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		//fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "OpenAL Error in initializing OpenAL Soft: ";
		std::cout << alGetString(err);
		std::cout << "\n";
		//return false;
	}
	
	std::cout << " Open AL Soft Initialized! \n";
#endif
}

void AudioMixer::setMusicVolume(uint8_t volume) noexcept
{
  volume = Mix_VolumeMusic(volume) & 0xFF;
  Settings::instance().musicVolume = volume;
}

void AudioMixer::setSoundEffectVolume(uint8_t volume) noexcept { Settings::instance().soundEffectsVolume = volume; }

void AudioMixer::playMusic(void) noexcept
{
  const std::shared_ptr<Music> themeMusic(new Music);
  m_musicObjects.push_back(themeMusic);

  std::string filePath = SDL_GetBasePath();
  filePath = filePath + "resources/audio/music/Cytopia OST 1.mp3";
  m_musicObjects[0]->loadFile(filePath);
  m_musicObjects[0]->play(-1);
}

void AudioMixer::mute(void) noexcept
{
  for (const auto &it : m_musicObjects)
  {
    it->stop();
    Settings::instance().playMusic = false;
    Settings::instance().playSoundEffects = false;
  }
}
