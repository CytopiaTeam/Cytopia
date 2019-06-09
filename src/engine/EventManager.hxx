#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include <SDL.h>
#include <set>

#include "Engine.hxx"
#include "UIManager.hxx"
#include "../util/Singleton.hxx"

class EventManager : public Singleton<EventManager>
{
public:
  EventManager() = default;
  ~EventManager() = default;

  void checkEvents(SDL_Event &event, Engine &engine);

  void registerTimer(Timer *timer);

private:
  UIManager &m_uiManager = UIManager::instance();

  UIElement *m_lastHoveredElement = nullptr;

  bool m_panning = false;
  bool m_skipLeftClick = false;
  bool m_tileInfoMode = false;
  Point pinchCenterCoords = {0, 0, 0, 0};
  Point m_clickDownCoords = {0, 0, 0, 0};
  Point m_highlitNode = {0, 0, 0, 0};
  std::vector<Point> m_highlightedNodes = {};
  std::vector<Timer *> timers;
};

#endif
