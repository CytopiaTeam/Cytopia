#include "MouseController.hxx"
#include "../events/MouseEvents.hxx"
#include "../controller/iMouseHandler.hxx"
#include "../util/iShape.hxx"
#include "../util/Rectangle.hxx"
#include "../util/LOG.hxx"

MouseController::MouseController(GameService::ServiceTuple & context, GlobalModel & model) :
	GameService(context), m_GlobalModel(model), m_LastHovered(nullptr)
{
	LOG(LOG_DEBUG) << "Created MouseController service";
}

MouseController::~MouseController()
{
  LOG(LOG_DEBUG) << "Destroying MouseController";
}

void MouseController::handleEvent(MousePositionEvent && event)
{
  Point2D point{event.xPosition, event.yPosition};
  if(m_LastHovered)
  {
    if(m_LastHovered->getShape().contains(point)) return;
    else m_LastHovered->onMouseLeave();
  }
  SpatialBlock block{event};
  if(m_SpatialMap.count(block) > 0)
  {
    for(auto handler : m_SpatialMap.at(block))
      if(handler->getShape().contains(point))
      {
        handler->onMouseHover();
        m_LastHovered = handler;
        return;
      }
  }
  m_LastHovered = nullptr;
}

void MouseController::addHandler(iMouseHandler * handler)
{
  Rectangle bounds = handler->getShape().getBounds();
  LOG(LOG_DEBUG) << "Adding a mouse handler for region " << bounds;
  auto [x1, y1] = bounds.p1();
  auto [x2, y2] = bounds.p2();
  constexpr int block = SpatialBlock::BlockSize::value;
  for(int i = x1 / block; i <= x2 / block; ++i)
    for(int j = y1 / block; j <= y2 / block; ++j)
    {
      LOG(LOG_DEBUG) << "Adding it to region (" << i << ", " << j << ")";
      m_SpatialMap[SpatialBlock{i, j}].push_back(handler);
    }
}

void MouseController::handleEvent(ClickEvent && event)
{
  if(m_LastHovered) m_LastHovered->onMouseClick(std::move(event));
}

void MouseController::handleEvent(ScrollEvent && event)
{
  if(m_LastHovered) m_LastHovered->onScroll(std::move(event));
}

void MouseController::handleEvent(ActivitySwitchEvent && )
{
  m_SpatialMap.clear();
  m_LastHovered = nullptr;
}

