#include "GameEvents.hxx"
#include <MapFunctions.hxx>
#include <Camera.hxx>
#include <LOG.hxx>

void GameEvents::unHighlightNodes()
{
  //   if (!m_isPuttingTile)
  //   {
  //     for (auto node : m_nodesToPlace)
  //     {
  //       MapFunctions::instance().unHighlightNode(node);
  //     }
  //     m_nodesToPlace.clear();
  //   }
  for (auto node : m_nodesToHighlight)
  {
    MapFunctions::instance().unHighlightNode(node);
  }
  m_nodesToHighlight.clear();
}

void GameEvents::processEvents(SDL_Event &event)
{
  switch (event.type)
  {
    // ----- MOUSE MOTION
  case SDL_MOUSEMOTION:
    if (mouseDownRight)
    {
      if ((event.motion.xrel == 0) && (event.motion.yrel == 0))
      {
        return;
      }
      Camera::instance().moveCamera(event.motion.xrel, event.motion.yrel);
      break;
    }
    break;

    // ----- MOUSEBUTTON DOWN
  case SDL_MOUSEBUTTONUP:
    //reset all states when we lift the mouse button
    mouseDown = false;
    mouseDownLeft = false;
    mouseDownRight = false;

    break;
  case SDL_MOUSEBUTTONDOWN:
    mouseDown = true;
    LOG(LOG_DEBUG) << "Hello from GameEvents()";
    switch (event.button.button)
    {
    case SDL_BUTTON_RIGHT:
      mouseDownRight = true;
      //   m_panning = true;
      //   m_cancelTileSelection = true;
      break;
    case SDL_BUTTON_LEFT:
      mouseDownLeft = true;
      break;
    }

  case SDL_MOUSEWHEEL:
    if (event.wheel.y > 0)
    {
      Camera::instance().increaseZoomLevel();
    }
    else if (event.wheel.y < 0)
    {
      Camera::instance().decreaseZoomLevel();
    }
    break;
  }
}
