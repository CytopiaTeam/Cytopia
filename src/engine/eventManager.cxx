#include "eventManager.hxx"

#include "basics/mapEdit.hxx"
#include "basics/timer.hxx"
#include "basics/settings.hxx"
#include "map.hxx"

#include "basics/log.hxx"
#include "basics/utils.hxx"
#include "ui/tooltip.hxx"

void EventManager::checkEvents(SDL_Event &event, Engine &engine)
{
  // check for UI events first
  Point mouseCoords;
  Point clickCoords;

  if (SDL_PollEvent(&event))
  {
    // check ui events first before checking any game event
    if (!dispatchUiEvents(event))
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
          uiManager.toggleGroupVisibility("PauseMenu");
          break;

        case SDLK_0:
          engine.toggleLayer(Engine::LAYER_GRID);
          break;
        case SDLK_F11:
          uiManager.toggleDebugMenu();
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
      case SDL_MOUSEBUTTONDOWN:
        mouseCoords = Point{event.button.x, event.button.y, 0, 0};
        clickCoords = Resources::convertScreenToIsoCoordinates(mouseCoords);
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          if (engine.isPointWithinBoundaries(clickCoords))
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
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          engine.centerScreenOnPoint(clickCoords);
        }
        break;

      case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0)
        {
          engine.increaseZoomLevel();
        }
        else if (event.wheel.y < 0)
        {
          engine.decreaseZoomLevel();
        }
        break;

      default:
        break;
      }
    }
  }
}

bool EventManager::dispatchUiEvents(SDL_Event &event)
{
  bool isMouseOverElement = false;
  bool isHovering = false;

  // the reversed draw order of the vector is  the Z-Order of the elements
  for (const std::shared_ptr<UiElement> &it : utils::ReverseIterator(uiManager.getAllUiElements()))
  {
    if (event.type == SDL_KEYDOWN)
    {
      if (it->onKeyDown(event))
      {
        return true;
      }
    }
    else if (it->isMouseOver(event.button.x, event.button.y) && it->isVisible())
    {
      isMouseOverElement = true;
      isHovering = it->isMouseOverHoverableArea(event.button.x, event.button.y);

      switch (event.type)
      {
      case SDL_MOUSEMOTION:
        if (it != lastHoveredElement && isHovering)
        {
          if (lastHoveredElement != nullptr)
          {
            lastHoveredElement->onMouseLeave(event);
          }
          it->onMouseEnter(event);
          lastHoveredElement = it;
        }
        else if (isMouseOverElement)
        {
          it->onMouseMove(event);
        }

        // handle tooltips
        if (!it->getTooltipText().empty())
        {
          uiManager.startTooltip(event, it->getTooltipText());
        }
        else
        {
          uiManager.stopTooltip();
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        it->onMouseButtonDown(event);
        break;
      case SDL_MOUSEBUTTONUP:
        it->onMouseButtonUp(event);
        break;
      }
      break; // break after the first element is found (Z-Order)
    }
  }

  if (!isHovering && lastHoveredElement != nullptr)
  {
    uiManager.stopTooltip();
    lastHoveredElement->onMouseLeave(event);
    lastHoveredElement = nullptr;
  }

  return isMouseOverElement;
}
