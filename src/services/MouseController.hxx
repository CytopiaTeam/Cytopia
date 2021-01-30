#ifndef MOUSE_CONTROLLER_HXX
#define MOUSE_CONTROLLER_HXX

#include <vector>
#include <unordered_map>

#include "../GameService.hxx"
#include "../events/MouseEvents.hxx"
#include "../events/UIEvents.hxx"
#include "../util/Point2D.hxx"
#include "../controller/iMouseHandler.hxx"

class MouseController : public GameService
{
public:
  MouseController(GameService::ServiceTuple &);
  ~MouseController();
  void addHandler(iMouseHandler *);
  void removeHandlers();

  void leftButtonDown(ClickEvent &&event);
  void middleButtonDown(ClickEvent &&event);
  void rightButtonDown(ClickEvent &&event);
  void leftButtonUp(ClickEvent &&event);
  void middleButtonUp(ClickEvent &&event);
  void rightButtonUp(ClickEvent &&event);
  void handleEvent(MousePositionEvent &&);
  void scroll(ScrollEvent &&);
  void handleEvent(ActivitySwitchEvent &&);

private:
  using HandlerList = std::vector<iMouseHandler *>;
  /**
   * @todo (ercadio): Use a vector instead of unordered_map
   */
  std::unordered_map<SpatialBlock, HandlerList> m_SpatialMap;
  iMouseHandler *m_LastHovered;
  iMouseHandler *m_Captured;

  void mouseMoved(MousePositionEvent &event);
  void mouseHover(MousePositionEvent &event);
};

#endif // MOUSE_CONTROLLER_HXX
