
template <typename StateType, typename... Args>
StateType & iActivity::createState(Args &&... args)
{
  StateType* state = new StateType(std::forward<Args>(args)..., GetService<UILoopMQ>());
  m_States.emplace_back(state);
  return *state;
}

template <typename ControllerType, typename... Args>
ControllerType & iActivity::createController(Args &&... args)
{
  ControllerType* controller = new ControllerType(std::forward<Args>(args)...);
  m_Controllers.emplace_back(controller);
  if constexpr(std::is_base_of<iMouseHandler, ControllerType>::value) {
    GetService<MouseController>().addHandler(controller);
  }
  return *controller;
}
