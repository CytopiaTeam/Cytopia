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
  ~EventManager();

  void checkEvents(SDL_Event &event, Engine &engine);
  void registerTimer(Timer *timer);
  /**
 * @brief Unhighlight Highlited Nodes.
 * This sets a node to be unhighlited.
 */
  void unHighlightNodes();
  void setWindow(class Window*);

private:
  UIManager &m_uiManager = UIManager::instance();

  UIElement *m_lastHoveredElement = nullptr;

  /// remember if placement is allowed from mousemove to mousedown
  bool m_placementAllowed = false;
  bool m_panning = false;
  bool m_skipLeftClick = false;
  bool m_tileInfoMode = false;
  bool m_cancelTileSelection = false; /// determines if a right click should cancel tile selection
  Point m_pinchCenterCoords = {0, 0, 0, 0};
  Point m_clickDownCoords = {0, 0, 0, 0};
  std::vector<Point> m_nodesToPlace = {};
  std::vector<Point> m_nodesToHighlight = {};
  std::vector<Timer *> m_timers;
  std::vector<Point> m_transparentBuildings;
  class Window * m_Window = nullptr;
};

#endif
