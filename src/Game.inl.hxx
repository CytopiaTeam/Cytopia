#ifdef USE_AUDIO
template <typename AudioEventType>
EnableIf<ContainsType<AudioEvents, AudioEventType>, void> Game::GameVisitor::operator()(AudioEventType &&event)
{
  GetService<AudioMixer>().handleEvent(std::move(event));
}
#endif // USE_AUDIO

template <typename ArgumentType> void Game::GameVisitor::operator()(const ArgumentType &&event)
{
  throw UnimplementedError(TRACE_INFO "Unimplemented Error");
}
