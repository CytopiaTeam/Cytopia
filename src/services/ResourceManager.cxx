#include "ResourceManager.hxx"

#ifdef USE_AUDIO
#include "AL/al.h"
#include "AL/alc.h"

//#include "ogg/ogg.h"
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
#include "Filesystem.hxx"

using ifstream = std::ifstream;
using nlohmann::json;

ResourceManager::ResourceManager() : m_Age(0), m_CacheSize(0)
{
#ifdef USE_AUDIO
  std::string jsonFileContent = fs::readFileAsString(Settings::instance().audioConfigJSONFile.get());
  json config_json = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (config_json.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().audioConfigJSONFile.get());

  m_audioConfig = config_json;
#endif // USE_AUDIO
}

#ifdef USE_AUDIO
int ResourceManager::LoadAudioWithOggVorbis(std::string path, DecodedAudioData &dAudioBuffer)
{

  OggVorbis_File vf;
  bool eof = false;
  int current_section;

  //small buffer to store small quantities of data
  char pcmout[4096];

#ifdef _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  //open file
  ifstream file(path, std::ifstream::in | std::ifstream::binary);

  //check if file is an vorbis ogg file
  if (ov_fopen(path.c_str(), &vf) < 0)
  {
    LOG(LOG_ERROR) << "Input does not appear to be an Ogg bitstream. \n" << stderr;
    return -1;
  }

  //readwhile end of file has not been reached yet
  while (!eof)
  {

    //read content from file
    // length of buffer
    // bingendianp bit packing = 0 = little endian
    // word size = 2 = 16-bit
    // sgned = 1 = signed

    //read to character buffer
    //number of bytes returned
    long ret = ov_read(&vf, pcmout, 4096, 0, 2, 1, &current_section);

    //check for errors
    //insert char data into audio buffer if there are no errors
    switch (ret)
    {
    case 0:
      eof = true;
      break;
    case OV_HOLE:
      LOG(LOG_ERROR) << "ERROR: OV_HOLE found in initial read of buffer\n";
      return -1;
      break;
    case OV_EBADLINK:
      LOG(LOG_ERROR) << "ERROR: OV_EBADLINK found in initial read of buffer\n";
      return -1;
      break;
    case OV_EINVAL:
      LOG(LOG_ERROR) << "ERROR: OV_EINVAL found in initial read of buffer\n";
      return -1;
      break;
    default:
      dAudioBuffer.char_data_vec.insert(dAudioBuffer.char_data_vec.end(), pcmout, pcmout + ret);
      break;
    }
  }

  //set number of bytes in buffer
  dAudioBuffer.nBytes = dAudioBuffer.char_data_vec.size() * sizeof(char);

  //get sample rate
  vorbis_info *vorbisInfo = ov_info(&vf, -1);
  dAudioBuffer.data_sample_rate = vorbisInfo->rate;

  //clear data
  ov_clear(&vf);

  LOG(LOG_DEBUG) << "Audio data read successful! Loaded into decoded audio buffer.\n";
  return 0;
}

void ResourceManager::fetch(SoundtrackID id)
{
  if (m_soundtracks.count(id) > 0)
  {
    m_soundtracks[id].age = m_Age++;
    return;
  }
  string filepath;
  bool isMusic = false;
  const AudioConfig::SoundtrackConfiguration *config;
  if (m_audioConfig.Music.count(id.get()) > 0)
  {
    config = &m_audioConfig.Music.at(id.get());
    isMusic = true;
  }
  else if (m_audioConfig.Sound.count(id.get()) > 0)
    config = &m_audioConfig.Sound.at(id.get());
  if (Settings::instance().audio3DStatus)
    filepath = fs::getBasePath() + config->monoFilePath;
  else
    filepath = fs::getBasePath() + config->stereoFilePath;
  LOG(LOG_INFO) << "Fetching " << id.get() << " at " << filepath;

  DecodedAudioData dataBuffer;
  if (LoadAudioWithOggVorbis(filepath.c_str(), dataBuffer) == -1)
  {
    throw AudioError(TRACE_INFO "Failed to read sound file with libvorbis.\n ");
  }

  if (dataBuffer.char_data_vec.empty())
    throw AudioError(TRACE_INFO "Could not read sound file: It is empty");

  m_CacheSize += sizeof(dataBuffer) + sizeof(Soundtrack) + sizeof(SoundtrackResource) + dataBuffer.nBytes;
  auto soundtrack = new Soundtrack{id, ChannelID{-1}, &dataBuffer, RepeatCount{0}, isMusic, false, true, true};
  m_soundtracks[id] = SoundtrackResource{SoundtrackUPtr{soundtrack}, m_Age++};

  LOG(LOG_INFO) << "Resource cache is now at " << (m_CacheSize / 1000000) << "MB";
  if (m_CacheSize > MAX_RESOURCE_BYTES::value)
    prune();
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
  all_ages.insert<AgeIt>(all_ages.cend(), AgeIterator{m_soundtracks.begin()}, AgeIterator{m_soundtracks.end()});
  std::nth_element(all_ages.begin(), all_ages.begin() + total_size, all_ages.end());
  uint32_t median = all_ages[total_size];
  for (auto it = m_soundtracks.begin(); it != m_soundtracks.end();)
  {
    if (it->second.age < median and !it->second.resource->isPlaying)
    {
      int sizeBytes = 0;
      alGetBufferi(it->second.resource->buffer, AL_SIZE, &sizeBytes);
      m_CacheSize -= sizeof(DecodedAudioData) + sizeof(Soundtrack) + sizeof(SoundtrackUPtr) + sizeBytes;
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
