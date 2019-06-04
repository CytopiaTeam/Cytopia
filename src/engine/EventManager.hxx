#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include <SDL.h>
#include <set>

#include "Engine.hxx"
#include "UIManager.hxx"

class EventManager
{
public:
  EventManager() = default;
  ~EventManager() = default;

  static EventManager& instance()
  {
      static EventManager evManager;
      return evManager;
  }

  void checkEvents(SDL_Event &event, Engine &engine);

  void registerTimer(Timer *timer);
  void removeTimer(Timer *timer);

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
  std::set<Timer*> timers;
  std::set<Timer*> removedTimers;
};

#endif