#include "ResourceManager.hxx"

#ifdef USE_OPENAL_SOFT
#include "AL/al.h"
#include "AL/alc.h"
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
  Mix_Chunk *chunk = Mix_LoadWAV(filepath.c_str());
  if (!chunk)
    throw AudioError(TRACE_INFO "Could not read sound file: " + string{Mix_GetError()});
  m_CacheSize += sizeof(Mix_Chunk) + sizeof(Soundtrack) + sizeof(SoundtrackResource) + chunk->alen;
  auto soundtrack = new Soundtrack{id, ChannelID{-1}, chunk, RepeatCount{0}, isMusic, false, true, true};
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

