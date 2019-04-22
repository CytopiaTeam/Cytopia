#include "eventManager.hxx"

#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/timer.hxx"
#include "basics/settings.hxx"
#include "map.hxx"

#include "basics/log.hxx"
#include "basics/utils.hxx"
#include "ui/widgets/tooltip.hxx"
#include "ui/menuGroups/MenuGroupBuild.hxx"

void EventManager::checkEvents(SDL_Event &event, Engine &engine)
{
  // check for UI events first
  SDL_Point mouseCoords;
  Point clickCoords;

  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
      engine.quitGame();
      break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
        // TODO: Toggle last opened menu or settings menu if nothing is open
        m_uiManager.toggleGroupVisibility("PauseMenu");
        break;

      case SDLK_0:
        engine.toggleLayer(Engine::LAYER_GRID);
        break;
      case SDLK_F11:
        m_uiManager.toggleDebugMenu();
        break;
      case SDLK_1:
        engine.toggleLayer(Engine::LAYER_FLOOR);
        break;

      case SDLK_2:
        engine.toggleLayer(Engine::LAYER_BUILDINGS);
        break;

      case SDLK_3:
        engine.toggleLayer(Engine::LAYER_SELECTION);
        break;

      case SDLK_f:
        engine.toggleFullScreen();
        break;

      case SDLK_b:
        LOG() << "Starting elevation Benchmark!";
        Timer benchmarkTimer;
        benchmarkTimer.start();
        for (int i = 0; i <= Settings::instance().settings.maxElevationHeight; i++)
        {
          engine.increaseHeight(Point{64, 64, 0, 0});
        }
        LOG() << "Done. Elevation took " << benchmarkTimer.getElapsedTime() << "ms";
        break;
      }
      break;

    case SDL_MULTIGESTURE:
      if (event.mgesture.numFingers == 2)
      {
        rightMouseButtonHeldDown = true;
        // check if we're pinching
        if (event.mgesture.dDist != 0)
        {
          Camera::setPinchDistance(event.mgesture.dDist * 15.0f);
          skipLeftClick = true;
          break;
        }

        if (rightMouseButtonHeldDown)
        {
          Camera::cameraOffset.x -= static_cast<int>(Settings::instance().settings.screenWidth * event.tfinger.dx);
          Camera::cameraOffset.y -= static_cast<int>(Settings::instance().settings.screenHeight * event.tfinger.dy);
          Camera::centerIsoCoordinates = convertScreenToIsoCoordinates(
              {Settings::instance().settings.screenWidth / 2, Settings::instance().settings.screenHeight / 2});
          Engine::instance().map->refresh();
          skipLeftClick = true;
          break;
        }
      }
      skipLeftClick = true;
      break;
    case SDL_MOUSEMOTION:
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElements())
      {
        // spawn tooltip timer, if we're over an UI Element
        if (it->isMouseOver(event.button.x, event.button.y) && !it->getUiElementData().tooltipText.empty())
        {
          m_uiManager.startTooltip(event, it->getUiElementData().tooltipText);
        }
        // if the mouse cursor left an element, we're not hovering any more and we need to reset the pointer to the last hovered element
        if (m_lastHoveredElement && !m_lastHoveredElement->isMouseOverHoverableArea(event.button.x, event.button.y))
        {
          // we're not hovering, so stop the tooltips
          m_uiManager.stopTooltip();
          // tell the previously hovered element we left it before resetting it
          m_lastHoveredElement->onMouseLeave(event);
          m_lastHoveredElement = nullptr;
          break;
        }
        // If we're over a UI element that has no click functionaliy, abort the event loop, so no clicks go through the UiElement.
        //Note: This is handled here because UIGroups have no dimensions, but are UiElements
        if (it->isMouseOverHoverableArea(event.button.x, event.button.y))
        {
          it->onMouseMove(event);
          // if the element we're hovering over is not the same as the stored "lastHoveredElement", update it
          if (it.get() != m_lastHoveredElement)
          {
            if (m_lastHoveredElement)
            {
              m_lastHoveredElement->onMouseLeave(event);
            }
            it->onMouseEnter(event);
            m_lastHoveredElement = it.get();
          }
          break;
        }
      }

      // game event handling
      mouseCoords = {event.button.x, event.button.y};
      clickCoords = convertScreenToIsoCoordinates(mouseCoords);
      if (rightMouseButtonHeldDown)
      {
        Camera::cameraOffset.x -= event.motion.xrel;
        Camera::cameraOffset.y -= event.motion.yrel;
        Camera::centerIsoCoordinates = convertScreenToIsoCoordinates(
            {Settings::instance().settings.screenWidth / 2, Settings::instance().settings.screenHeight / 2});
        Engine::instance().map->refresh();
      }
      else
      {
        engine.map->highlightNode(clickCoords);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      skipLeftClick = false;
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElementsForEventHandling())
      {
        // only check visible elements
        if (it->isVisible())
        {
          if (it->onMouseButtonDown(event))
          {
            break;
          }
        }
      }

      if (event.button.button == SDL_BUTTON_RIGHT)
      {
        rightMouseButtonHeldDown = true;
      }
      break;

    case SDL_MOUSEBUTTONUP:
      rightMouseButtonHeldDown = false;
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElementsForEventHandling())
      {
        // only check visible elements
        if (it->isVisible())
        {
          // first, check if the element is a group and send the event
          if (it->onMouseButtonUp(event))
          {
            skipLeftClick = true;
            break;
          }
          // If we're over a UI element that has no click functionaliy, abort the event loop, so no clicks go through the UiElement.
          //Note: This is handled here because UIGroups have no dimensions, but are UiElements
          if (it->isMouseOver(event.button.x, event.button.y))
          {
            skipLeftClick = true;
          }
        }
      }
      // If we're over a ui element, don't handle game events
      if (skipLeftClick)
      {
        skipLeftClick = true;
        break;
      }

      // game event handling
      mouseCoords = {event.button.x, event.button.y};
      clickCoords = convertScreenToIsoCoordinates(mouseCoords);

      if (event.button.button == SDL_BUTTON_LEFT)
      {
        if (isPointWithinMapBoundaries(clickCoords))
        {
          if (terrainEditMode == TerrainEdit::RAISE)
          {
            engine.increaseHeight(clickCoords);
          }
          else if (terrainEditMode == TerrainEdit::LOWER)
          {
            engine.decreaseHeight(clickCoords);
          }
          else if (!tileTypeEditMode.empty())
          {
            engine.setTileIDOfNode(clickCoords, tileTypeEditMode);
          }
          else if (demolishMode)
          {
            engine.demolishNode(clickCoords);
          }
          else
          {
            LOG() << "CLICKED - Iso Coords: " << clickCoords.x << ", " << clickCoords.y;
          }
        }
      }

      break;
      break;
    case SDL_MOUSEWHEEL:
      if (event.wheel.y > 0)
      {
        Camera::increaseZoomLevel();
      }
      else if (event.wheel.y < 0)
      {
        Camera::decreaseZoomLevel();
      }
      break;

    default:
      break;
    }
  }
}
