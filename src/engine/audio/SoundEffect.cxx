#include "SoundEffect.hxx"

#include "../basics/LOG.hxx"
#include "../basics/Settings.hxx"

SoundEffect::SoundEffect(const std::string &filename) : m_playSoundEffect(Settings::instance().playSoundEffects)
{
  loadFile(filename);

}

SoundEffect::~SoundEffect()
{
  if (m_soundEffect)
  {
    Mix_FreeChunk(m_soundEffect);
  }
  
  #ifdef USE_OPENAL_SOFT
  if(source != 0)
	{
		alDeleteSources(1, &source);
	}
	
  if(buffer != 0)
  {
	   alDeleteBuffers(1, &buffer);
  }
  #endif
}

void SoundEffect::loadFile(const std::string &filename)
{
  m_soundEffect = Mix_LoadWAV(filename.c_str());
  if (!m_soundEffect)
  {
    LOG(LOG_ERROR) << "Failed to load audio file " << filename << "\n" << Mix_GetError();
    LOG() << "Disabled sound effect playback!";
    m_playSoundEffect = false;
  }
  #ifdef USE_OPENAL_SOFT
  source = 0;
  buffer = 0;
  
  //initialize source
  alGenSources(1, &source);
  alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
  assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source.");
  
  //initialize buffer
 
  ALuint tempBuffer = 0; //initialize temp buffer
			
  alGenBuffers(1, &tempBuffer);//request 1 buffer	
  //set buffer data
  //ALenum variable for format of sound sample e.g. mono,stereo, 8-bit 16-bit
  ALenum format = AL_FORMAT_STEREO16; //assuming sound effect is converted to MIX_DEFAULT_FORMAT default stereo signed 16-bit
  
  alBufferData(tempBuffer, format, m_soundEffect->abuf, m_soundEffect->alen * sizeof(int16_t), MIX_DEFAULT_FREQUENCY);
  
 
  /* Check if an error occured, and clean up if so. */
  ALenum err = alGetError();


  if(err != AL_NO_ERROR)
  {
	  fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
	  if(tempBuffer && alIsBuffer(tempBuffer)){alDeleteBuffers(1, &tempBuffer);}
  }
  else
  { 
	  buffer = tempBuffer;
	  std::cout << "buffer succesfully loaded for sound effect. \n";
	  alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	  alSource3f(source, AL_POSITION, 0.0f, 0.0f, -1.0f);
	  alSourcei(source, AL_BUFFER, buffer);
	  assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source with buffer.");
  }
  #endif
}

void SoundEffect::play(int channel, Sint16 angle, Uint8 distance, int loops) const
{
  if (m_playSoundEffect)
  {

    if (m_soundEffect)
    {
	  #ifdef USE_OPENAL_SOFT
	  if(Settings::instance().audioChannels >= 3)
	  {
		  std::cout << "OpenAL Soft version of play sound effect called! \n";
	  }
	  #endif
	  
      const int currentChannel = Mix_PlayChannel(channel, m_soundEffect, loops);

      if (currentChannel == -1)
      {
        LOG(LOG_ERROR) << "Failed to play audio file\n" << Mix_GetError();
      }

      // for stereo set the position of the sound effect
      if (Settings::instance().audioChannels == 2 && !Mix_SetPosition(currentChannel, angle, distance))
      {
        LOG(LOG_ERROR) << "Failed to set audio position!\n" << Mix_GetError();
      }
    }
    else
    {
      LOG(LOG_ERROR) << "No sound effect file is loaded but the function play() has been called!\n";
    }
    
    //else if in binaural audio mode
    
  }
}

void SoundEffect::stop(int channel) const
{
  if (m_playSoundEffect)
  {
    Mix_HaltChannel(channel);
  }
}

bool SoundEffect::isPlaying() const
{
  if (m_playSoundEffect)
  {
    // returns amount of playing audiochannels
    return Mix_Playing(-1) != 0;
  }
  return false;
}

void SoundEffect::enableSoundEffects(bool enabled) { m_playSoundEffect = enabled; }
