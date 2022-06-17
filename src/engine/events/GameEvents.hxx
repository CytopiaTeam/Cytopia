#ifndef GAME_EVENTS_HXX_
#define GAME_EVENTS_HXX_

#include <Singleton.hxx>
#include <Point.hxx>

class GameEvents : public Singleton<GameEvents>
{
public:
  GameEvents() = default;

  void unHighlightNodes();
  void addNodesToHighlight();

  void processEvents(SDL_Event &event);

private:
  std::vector<Point> m_nodesToHighlight = {};

  bool mouseDown=false;
  bool mouseDownLeft=false;
  bool mouseDownRight=false;
};

#endif