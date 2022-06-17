#include "GameEvents.hxx"
#include <MapFunctions.hxx>
#include <Camera.hxx>
#include "../basics/isoMath.hxx"

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
  if (!MapFunctions::instance().getMap())
  { // abort if there's no map to interact with
    return;
  }

  SDL_Point mouseScreenCoords = {event.button.x, event.button.y};
  Point mouseIsoCoords = convertScreenToIsoCoordinates(mouseScreenCoords);

  switch (event.type)
  {
    // ----- MOUSE MOTION
  case SDL_MOUSEMOTION:
    if (mouseDownRight)
    { // if the right mouse button is being held down, pan the map
      if ((event.motion.xrel == 0) && (event.motion.yrel == 0))
      {
        return;
      }
      Camera::instance().moveCamera(event.motion.xrel, event.motion.yrel);
      break; // when we're panning, don't handle other events
    }

    // clear highlighting, we'll rebuild it now
    unHighlightNodes();

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
    break;
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
