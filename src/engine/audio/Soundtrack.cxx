#include "Soundtrack.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"
#include "../basics/Settings.hxx"

Soundtrack::Soundtrack(SoundtrackID id, ChannelID channelID, Mix_Chunk *chunks, RepeatCount repeat, bool isMusic, bool isPlaying, bool isPlayable, bool isTriggerable)
    : ID(id), Channel(channelID), Chunks(chunks), Loop(repeat), isMusic(isMusic), isPlaying(isPlaying), isPlayable(isPlayable),
      isTriggerable(isTriggerable)
{
  /* initialize buffer */
  alGenBuffers(1, &buffer);
  ALenum errorCode = alGetError();
  if (errorCode != AL_NO_ERROR)
    throw AudioError(TRACE_INFO "Could not create buffers: Error " + std::to_string(errorCode));

  /* set buffer data */
  ALenum format = Settings::instance().audio3DStatus ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  /* parameters: buffer, format, data, sample length, frequency(sample rate) */
  alBufferData(buffer, format, Chunks->abuf, Chunks->alen,
               44100);

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
  if (Chunks)
    Mix_FreeChunk(Chunks);

#ifdef USE_OPENAL_SOFT
  if (buffer)
    alDeleteBuffers(1, &buffer);
  if (source)
    alDeleteSources(1, &source);
#endif
}