#include "Soundtrack.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"
#include "../basics/Settings.hxx"

Soundtrack::Soundtrack(SoundtrackID id, ChannelID channelID, DecodedAudioData *dAudioData, RepeatCount repeat, bool isMusic,
                       bool isPlaying, bool isPlayable, bool isTriggerable)
    : ID(id), Channel(channelID), Loop(repeat), isMusic(isMusic), isPlaying(isPlaying), isPlayable(isPlayable),
      isTriggerable(isTriggerable),
#ifdef USE_AUDIO
      source(0), buffer(0)
#else  // USE_OPENAL_SOFT
      dAudioDataBuffer(dAudioData)
#endif // USE_OPENAL_SOFT
{
  /* initialize buffer */
  alGenBuffers(1, &buffer);
  ALenum errorCode = alGetError();
  if (errorCode != AL_NO_ERROR)
    throw AudioError(TRACE_INFO "Could not create buffers: Error " + std::to_string(errorCode));

  /* set buffer data */
  ALenum format = Settings::instance().audio3DStatus ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  /* parameters: buffer, format, data, sample length, frequency(sample rate) */
  //alBufferData(buffer, format, chunks->abuf, chunks->alen,
  //             44100);

  alBufferData(buffer, format, dAudioData->char_data_vec.data(), dAudioData->nBytes, dAudioData->data_sample_rate);

  errorCode = alGetError();
  if (errorCode != AL_NO_ERROR)
    throw AudioError(TRACE_INFO "Failed to load audio data into buffer: Error " + std::to_string(errorCode));

  /* delete chunks */
  //Mix_FreeChunk(chunks);

  /* initialize source */
  alGenSources(1, &source);
  alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
  errorCode = alGetError();
  if (errorCode != AL_NO_ERROR)
    throw AudioError(TRACE_INFO "Failed to setup sound source: Error " + std::to_string(errorCode));

  /* attach buffer to source */
  alSourcei(source, AL_BUFFER, buffer);
}

Soundtrack::~Soundtrack()
{
#ifdef USE_OPENAL_SOFT
  if (alIsSource(source))
    alDeleteSources(1, &source);
  if (alIsBuffer(buffer))
    alDeleteBuffers(1, &buffer);
//#else // USE_OPENAL_SOFT
//if (Chunks)
//  Mix_FreeChunk(Chunks);
#endif // USE_OPENAL_SOFT
}
