#include "ResourceManager.hxx"

#ifdef USE_OPENAL_SOFT
#include "AL/al.h"
#include "AL/alc.h"

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/vorbisenc.h"


#include <cstdio>
#include <cstdlib>
#include <cmath>

//windows vorbisfile audio decoding, 
//need to declare these libraries so that we can set stdin/stdout to binary
//for audio decoding on windows
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#endif

#include "../util/Exception.hxx"
#include "../util/LOG.hxx"

#include "common/JsonSerialization.hxx"
#include <fstream>


using ifstream = std::ifstream;
using nlohmann::json;

ResourceManager::ResourceManager(GameService::ServiceTuple &services) : GameService(services), m_Age(0), m_CacheSize(0)
{
#ifdef USE_AUDIO
  string fName = SDL_GetBasePath() + Settings::instance().audioConfigJSONFile.get();
  ifstream ifs(fName);
  if (!ifs)
    throw ConfigurationError(TRACE_INFO "Couldn't open file " + fName);
  json config_json;
  ifs >> config_json;
  m_audioConfig = config_json;
#endif // USE_AUDIO
}


//#ifdef USE_OPENAL_SOFT
int LoadAudioWithOggVorbis(std::string path, DecodedAudioData& dAudioBuffer)
{
  //0 means success
  //-1 means failure
  
  OggVorbis_File vf;
  bool eof = false;
  int current_section;
  
  //small buffer to store small quantities of data
  char pcmout[4096];

#ifdef _WIN32
  _setmode( _fileno( stdin ), _O_BINARY );
  _setmode( _fileno( stdout ), _O_BINARY );
#endif

  //open file
  ifstream file(path,  std::ifstream::in |  std::ifstream::binary);
  
  //check if file is an vorbis ogg file
  if(ov_fopen(path.c_str(),&vf) < 0) 
  {
      fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
      return -1;
  }

  {
    char **ptr=ov_comment(&vf,-1)->user_comments;
    vorbis_info *vi=ov_info(&vf,-1);
    while(*ptr){
      fprintf(stderr,"%s\n",*ptr);
      ++ptr;
    }
    fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
    fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
  }
  
  //readwhile end of file has not been reached yet
  while(!eof)
  {
	
	//read content from file
    long ret = ov_read(&vf,pcmout,sizeof(pcmout),0,2,1,&current_section);
    
    //if there is no more content
    if (ret == 0) 
    {
      /* EOF */
      eof = true;
    } 
    //if there is an error
    else if (ret < 0) 
    {
      /* error in the stream.*/
      return -1;
    }
    //else if no error
    else
    {
		uint16_t vals[4096];
		
		//convert from char to uint and push into buffer
		
		for(size_t i=0; i < 4096; i++)
		{
			vals[i] = (uint16_t)pcmout[i];
			dAudioBuffer.data_vec.push_back(vals[i]);
		}
	}
  }
  
  //clear data
  ov_clear(&vf);
  
  //set number of bytes in buffer
  dAudioBuffer.nBytes = dAudioBuffer.data_vec.size() * sizeof(uint16_t);
      
  LOG(LOG_DEBUG) << "Load Successful!\n";
  return 0;
}

//#endif

#ifdef USE_AUDIO
void ResourceManager::fetch(SoundtrackID id)
{
  if(m_soundtracks.count(id) > 0)
  {
    m_soundtracks[id].age = m_Age++;
    return;
  }
  string filepath;
  bool isMusic = false;
  const AudioConfig::SoundtrackConfiguration * config;
  if(m_audioConfig.Music.count(id.get()) > 0)
  {
    config = &m_audioConfig.Music.at(id.get());
    isMusic = true;
  }
  else if (m_audioConfig.Sound.count(id.get()) > 0)
    config = &m_audioConfig.Sound.at(id.get());
  if (Settings::instance().audio3DStatus)
    filepath = SDL_GetBasePath() + config->monoFilePath;
  else
    filepath = SDL_GetBasePath() + config->stereoFilePath;
  LOG(LOG_INFO) << "Fetching " << id.get() << " at " << filepath;
  //Mix_Chunk *chunk = Mix_LoadWAV(filepath.c_str());
  DecodedAudioData dataBuffer;
  if( LoadAudioWithOggVorbis(filepath.c_str(),dataBuffer) == -1)
  {
	  throw AudioError(TRACE_INFO "Failed to read sound file with libogg.\n ");
  }
   
  if (dataBuffer.data_vec.size() == 0)
    throw AudioError(TRACE_INFO "Could not read sound file: " + string{Mix_GetError()});
  m_CacheSize += sizeof(dataBuffer) + sizeof(Soundtrack) + sizeof(SoundtrackResource) + dataBuffer.nBytes;
  auto soundtrack = new Soundtrack{id, ChannelID{-1}, &dataBuffer, RepeatCount{0}, isMusic, false, true, true};
  m_soundtracks[id] = SoundtrackResource{ SoundtrackUPtr{soundtrack}, m_Age++ };
  LOG(LOG_INFO) << "Resource cache is now at " << (m_CacheSize / 1000000) << "MB";
  if (m_CacheSize > MAX_RESOURCE_BYTES::value) prune();
}
#endif // USE_AUDIO

void ResourceManager::prune()
{
#ifdef USE_AUDIO
  size_t total_size = 0;
  total_size += m_soundtracks.size();
  /* We evict a half of kept resources */
  total_size /= 2;
  std::vector<uint32_t> all_ages;
  using AgeIt = AgeIterator<Mapping<SoundtrackID, SoundtrackResource>::iterator>;
  all_ages.insert<AgeIt>(
      all_ages.cend(),
      AgeIterator{m_soundtracks.begin()}, 
      AgeIterator{m_soundtracks.end()});
  std::nth_element(all_ages.begin(), all_ages.begin() + total_size, all_ages.end());
  uint32_t median = all_ages[total_size];
  for(auto it = m_soundtracks.begin(); it != m_soundtracks.end();)
  {
    if(it->second.age < median and !it->second.resource->isPlaying) 
    {
      int sizeBytes = 0;
      alGetBufferi(it->second.resource->buffer, AL_SIZE, &sizeBytes);
      m_CacheSize -= sizeof(Mix_Chunk) + sizeof(Soundtrack) + sizeof(SoundtrackUPtr) + sizeBytes;
      it = m_soundtracks.erase(it);
    }
    else
    {
      it->second.age = 0;
      it++; 
    }
  }
#endif // USE_AUDIO
  m_Age = 0;
  LOG(LOG_INFO) << "After eviction, cache is " << (m_CacheSize / 1000000) << "MB";
}

