#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include <SDL.h>
#include <set>
#include <optional>

#include <Point.hxx>
#include "../util/Singleton.hxx"
#include "common/enums.hxx"

namespace detail
{
std::optional<PlacementMode> updatePlacementMode(Layer topMostActiveLayer);
}

class UIElement;

class EventManager : public Singleton<EventManager>
{
public:
  EventManager() = default;
  ~EventManager() = default;

  void checkEvents(SDL_Event &event);
  /**
 * @brief Unhighlight highlighted Nodes.
 * @details This sets a node to be unhighlighted.
 */
  void unHighlightNodes();
  void pickTileUnderCursor(Point mouseIsoCoords);

private:
  UIElement *m_lastHoveredElement = nullptr;

  /// remember if placement is allowed from mousemove to mousedown
  bool m_placementAllowed = false;
  bool m_panning = false;
  bool m_tileInfoMode = false;
  bool m_isPuttingTile = false;       ///< determines if putting tile action is being performed
  bool m_cancelTileSelection = false; ///< determines if a right click should cancel tile selection
  Point m_pinchCenterCoords = {0, 0, 0, 0};
  Point m_clickDownCoords = {0, 0, 0, 0};
  std::vector<Point> m_nodesToPlace = {};
  std::vector<Point> m_nodesToHighlight = {};
  std::vector<Point> m_transparentBuildings;
};

#endif
