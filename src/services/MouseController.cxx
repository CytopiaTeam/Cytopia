#include "MouseController.hxx"
#include "../util/LOG.hxx"
#include "../util/iShape.hxx"
#include "../util/Rectangle.hxx"
#include "../events/MouseEvents.hxx"

MouseController::MouseController(GameService::ServiceTuple &context)
    : GameService(context), m_LastHovered(nullptr), m_Captured(nullptr)
{
  debug_scope {
    LOG(LOG_DEBUG) << "Created MouseController service";
  }
}

MouseController::~MouseController() { 
  debug_scope {
    LOG(LOG_DEBUG) << "Destroying MouseController";
  }
}

void MouseController::mouseMoved(MousePositionEvent &event)
{
  iMouseHandler *pViewCtrl = m_LastHovered;
  if (m_Captured)
  {
    pViewCtrl = m_Captured;
  }

  if (pViewCtrl)
  {
    auto [x1, y1] = pViewCtrl->getShape().getBounds().p1();
    event.xPosition -= x1;
    event.yPosition -= y1;
    pViewCtrl->onMouseMove(std::move(event));
  }
}

void MouseController::mouseHover(MousePositionEvent &event)
{
  Point2D point{event.xPosition, event.yPosition};

  if (m_LastHovered)
  {
    if (m_LastHovered->getShape().contains(point))
    {
      return;
    }
    else
    {
      m_LastHovered->onMouseLeave();
    }
  }

  SpatialBlock block{event};

  if (m_SpatialMap.count(block) > 0)
  {
    for (auto handler : m_SpatialMap.at(block))
    {
      if (handler->getShape().contains(point))
      {
        handler->onMouseHover();
        m_LastHovered = handler;
        return;
      }
    }
  }

  m_LastHovered = nullptr;
}

void MouseController::handleEvent(MousePositionEvent &&event)
{
  mouseHover(event);
  mouseMoved(event);
}

void MouseController::addHandler(iMouseHandler *handler)
{
  Rectangle bounds = handler->getShape().getBounds();
  debug_scope {
    LOG(LOG_DEBUG) << "Adding a mouse handler for region " << bounds;
  }
  auto [x1, y1] = bounds.p1();
  auto [x2, y2] = bounds.p2();
  constexpr int block = SpatialBlock::BlockSize::value;
  for (int i = x1 / block; i <= x2 / block; ++i)
    for (int j = y1 / block; j <= y2 / block; ++j)
    {
      debug_scope {
        LOG(LOG_DEBUG) << "Adding it to region (" << i << ", " << j << ")";
      }
      m_SpatialMap[SpatialBlock{i, j}].push_back(handler);
    }
}

void MouseController::removeHandlers()
{
  m_SpatialMap.clear();
  m_LastHovered = nullptr;
}

void MouseController::leftButtonDown(ClickEvent &&event)
{
  if (m_LastHovered)
  {
    m_Captured = m_LastHovered;
    m_Captured->onMouseLeftButtonDown(std::move(event));
  }
}

void MouseController::leftButtonUp(ClickEvent &&event)
{
  if (m_Captured)
  {

    m_Captured->onMouseLeftButtonUp(std::move(event));
    m_Captured = nullptr;
  }
}

void MouseController::middleButtonDown(ClickEvent &&event)
{
  if (m_LastHovered)
  {
    m_LastHovered->onMouseMiddleButtonDown(std::move(event));
  }
}

void MouseController::middleButtonUp(ClickEvent &&event)
{
  if (m_LastHovered)
  {
    m_LastHovered->onMouseMiddleButtonDown(std::move(event));
  }
}

void MouseController::rightButtonDown(ClickEvent &&event)
{
  if (m_LastHovered)
  {
    m_LastHovered->onMouseRightButtonDown(std::move(event));
  }
}

void MouseController::rightButtonUp(ClickEvent &&event)
{
  if (m_LastHovered)
  {
    m_LastHovered->onMouseRightButtonDown(std::move(event));
  }
}

void MouseController::scroll(ScrollEvent &&event)
{
  if (m_LastHovered)
    m_LastHovered->onScroll(std::move(event));
}

void MouseController::handleEvent(ActivitySwitchEvent &&) { removeHandlers(); }
