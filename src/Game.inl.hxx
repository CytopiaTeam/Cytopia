
template<typename ArgumentType>
void Game::UIVisitor::operator()(ArgumentType&& event)
{
  static_assert(std::is_void_v<std::void_t<ArgumentType>>, "UIVisitor does not know how to handle this event. You must specialize the functor");
}

template<typename TransitiveType>
void Game::UIVisitor::operator()(TransitiveStateChange<TransitiveType>&& event)
{
  if(auto uiTarget = event.target.lock())
  {
    uiTarget->update(event.data);
  }
}

template <typename AudioEventType>
EnableIf<ContainsType<Game::AudioEvents, AudioEventType>, void>
Game::GameVisitor::operator()(AudioEventType&& event)
{
  GetService<AudioMixer>().handleEvent(std::move(event));
}

template<typename ArgumentType>
void Game::GameVisitor::operator()(const ArgumentType&& event)
{
  throw UnimplementedError(TRACE_INFO "Unimplemented Error");
}
