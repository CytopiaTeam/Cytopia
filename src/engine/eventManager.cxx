#include "eventManager.hxx"

void EventManager::checkEvents(SDL_Event &event, Engine &engine)
{
  // check for UI events first
  Point mouseCoords;
  Point clickCoords;

  if (SDL_PollEvent(&event))
  {
    // check ui events first before checking any game event
    if (!handleUIEvents(event) && !isHandlingMouseEvents)
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

        case SDLK_1:
          engine.toggleLayer(Engine::LAYER_FLOOR);
          break;

        case SDLK_2:
          engine.toggleLayer(Engine::LAYER_BUILDINGS);
          break;

        case SDLK_3:
          engine.toggleLayer(Engine::LAYER_SELECTION);
          break;
        case SDLK_r:
          engine.rotateMap();
          break;
        case SDLK_f:
          engine.toggleFullScreen();
          break;
        case SDLK_b:
          LOG() << "Starting elevation Benchmark!";
          Timer benchmarkTimer;
          benchmarkTimer.start();
          for (int i = 0; i <= Settings::Instance().settings.maxElevationHeight; i++)
          {
            engine.increaseHeightOfCell(Point{64, 64, 0, 0});
          }
          LOG() << "Done. Elevation took " << benchmarkTimer.getDeltaTime() << "ms";
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
            if (Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE)
              engine.increaseHeightOfCell(clickCoords);
            else if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
            {
              engine.decreaseHeightOfCell(clickCoords);
            }
            else
              LOG() << "CLICKED - Iso Coords: " << clickCoords.x << ", " << clickCoords.y << " and height: " << clickCoords.height;
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

bool EventManager::handleUIEvents(SDL_Event &event)
{
  bool handled = false;
  std::shared_ptr<UiElement> clickedElement = uiManager.getClickedUIElement(event.button.x, event.button.y);

  switch (event.type)
  {
  case SDL_MOUSEMOTION:
    if (clickedElement)
    {
      if (!clickedElement->getTooltipText().empty())
      {

        _tooltip->setText(clickedElement->getTooltipText());
        _tooltip->setPosition(event.button.x - _tooltip->getUiElementRect().w / 2,
                              event.button.y - _tooltip->getUiElementRect().h);
        _tooltip->setVisibility(true);
        _tooltip->startTimer();
      }
      else
      {
        _tooltip->setVisibility(false);
      }
    }
    else
    {
      _tooltip->setVisibility(false);
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (event.button.button == SDL_BUTTON_LEFT && clickedElement)
    {
      isHandlingMouseEvents = true;
    }
    break;
  case SDL_MOUSEBUTTONUP:
    isHandlingMouseEvents = false;

    if (event.button.button == SDL_BUTTON_LEFT)
    {
      if (clickedElement)
      {
        switch (clickedElement->getActionID())
        {
        case 0:
          break;
        case 1:
          UIManager::Instance().toggleGroupVisibility(clickedElement->getParentID());
          break;
        case 2:
          if (Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE)
          {
            Resources::setTerrainEditMode(Resources::NO_TERRAIN_EDIT);
          }
          else
          {
            Resources::setTerrainEditMode(Resources::TERRAIN_RAISE);
          }
          break;
        case 3:
          if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
          {
            Resources::setTerrainEditMode(Resources::NO_TERRAIN_EDIT);
          }
          else
          {
            Resources::setTerrainEditMode(Resources::TERRAIN_LOWER);
          }
          break;
        case 4:
          Engine::Instance().quitGame();
          break;
        default:
          break;
        }
        handled = true;
      }
    }
    break;
  default:
    break;
  }

  // return if the event was handled here
  return handled;
}