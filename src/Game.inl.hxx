
template <typename ArgumentType> void Game::UIVisitor::operator()(ArgumentType &&event)
{
  static_assert(
      sizeof(ArgumentType) < 0,
                "UIVisitor does not know how to handle this event. You must specialize the functor");
}


#ifdef USE_AUDIO
template <typename AudioEventType>
EnableIf<ContainsType<AudioEvents, AudioEventType>, void> Game::GameVisitor::operator()(AudioEventType &&event)
{
  GetService<AudioMixer>().handleEvent(std::move(event));
}
#endif // USE_AUDIO

template <typename ArgumentType> void Game::GameVisitor::operator()(const ArgumentType &&event)
{
  static_assert(
      sizeof(ArgumentType) < 0,
                "GameVisitor does not know how to handle this event. You must specialize the functor");
}
