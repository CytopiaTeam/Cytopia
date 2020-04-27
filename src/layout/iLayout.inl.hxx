
template <typename ControllerType, typename... Args>
ControllerType & iLayout::createController(Args &&... args)
{
	ControllerType* controller = new ControllerType(std::forward<decltype(args)>(args)...);
  return *(m_Controllers.emplace_back(controller));
}

