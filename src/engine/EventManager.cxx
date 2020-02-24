#include "EventManager.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "common/enums.hxx"
#include "map/MapLayers.hxx"
#include "Map.hxx"
#include "Sprite.hxx"

#include "LOG.hxx"

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

void EventManager::unHighlighNodes(Engine &engine)
{
  for (size_t i = 0; i < m_highlightedNodes.size(); i++)
  {
    engine.map->unHighlightNode(m_highlightedNodes[i]);
  }
  for (size_t i = 0; i < m_highlightedObjectNodes.size(); i++)
  {
    engine.map->unHighlightNode(m_highlightedObjectNodes[i]);
  }
  m_highlightedNodes.clear();
  m_highlightedObjectNodes.clear();
}

void EventManager::checkEvents(SDL_Event &event, Engine &engine)
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("EventManager", "checkEvents", MP_BEIGE);
#endif
  // check for UI events first
  SDL_Point mouseCoords;
  Point clickCoords{};

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
        break;
      case SDLK_F11:
        m_uiManager.toggleDebugMenu();
        break;
      case SDLK_1:
        MapLayers::toggleLayer(Layer::TERRAIN);
        break;
      case SDLK_2:
        MapLayers::toggleLayer(Layer::BUILDINGS);
        break;
      case SDLK_3:
        break;
      case SDLK_i:
        m_tileInfoMode = !m_tileInfoMode;
        break;
      case SDLK_h:
        // TODO: This is only temporary until the new UI is ready. Remove this afterwards
        Settings::instance().drawUI = !Settings::instance().drawUI;
        break;
      case SDLK_f:
        engine.toggleFullScreen();
        break;
      case SDLK_w:
        if (Camera::cameraOffset.y > -2 * Settings::instance().screenHeight * Camera::zoomLevel)
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::cameraOffset.y -= (Settings::instance().screenHeight / 16);
            // set the center coordinates for scrolling
            Camera::centerIsoCoordinates =
                convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
            Engine::instance().map->refresh();
          }
        }
        break;
      case SDLK_a:
        if (Camera::cameraOffset.x > -0.25 * Settings::instance().screenWidth * Camera::zoomLevel)
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::cameraOffset.x -= (Settings::instance().screenWidth / 16);
            // set the center coordinates for scrolling
            Camera::centerIsoCoordinates =
                convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
            Engine::instance().map->refresh();
          }
        }
        break;

      case SDLK_s:
        if (Camera::cameraOffset.y < 1.25 * Settings::instance().screenHeight * Camera::zoomLevel)
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::cameraOffset.y += (Settings::instance().screenHeight / 16);
            // set the center coordinates for scrolling
            Camera::centerIsoCoordinates =
                convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
            Engine::instance().map->refresh();
          }
        }
        break;

      case SDLK_d:
        if (Camera::cameraOffset.x < 5 * Settings::instance().screenWidth * Camera::zoomLevel)
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::cameraOffset.x += (Settings::instance().screenWidth / 16);
            // set the center coordinates for scrolling
            Camera::centerIsoCoordinates =
                convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
            Engine::instance().map->refresh();
          }
        }
        break;

      default:
        break;
      }
      break;

    case SDL_MULTIGESTURE:
      if (event.mgesture.numFingers == 2)
      {
        m_panning = true;
        // check if we're pinching
        if (event.mgesture.dDist != 0)
        {
          // store pinchCenterCoords so they stay the same for all zoom levels
          if (pinchCenterCoords.x == 0 && pinchCenterCoords.y == 0)
          {
            pinchCenterCoords =
                convertScreenToIsoCoordinates({static_cast<int>(event.mgesture.x * Settings::instance().screenWidth),
                                               static_cast<int>(event.mgesture.y * Settings::instance().screenHeight)});
          }
          Camera::setPinchDistance(event.mgesture.dDist * 15.0F, pinchCenterCoords.x, pinchCenterCoords.y);
          m_skipLeftClick = true;
          break;
        }

        if (m_panning)
        {
          Camera::cameraOffset.x -= static_cast<int>(Settings::instance().screenWidth * event.tfinger.dx);
          Camera::cameraOffset.y -= static_cast<int>(Settings::instance().screenHeight * event.tfinger.dy);
          Camera::centerIsoCoordinates =
              convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
          Engine::instance().map->refresh();
          m_skipLeftClick = true;
          break;
        }
      }
      m_skipLeftClick = true;
      break;
    case SDL_MOUSEMOTION:
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElements())
      {
        // if element isn't visible then don't event check it
        if (it->isVisible())
        {
          // spawn tooltip timer, if we're over an UI Element
          if (it->isMouseOver(event.button.x, event.button.y) && !it->getUiElementData().tooltipText.empty())
          {
            m_uiManager.startTooltip(event, it->getUiElementData().tooltipText);
          }
          // if the mouse cursor left an element, we're not hovering any more and we need to reset the pointer to null
          if (m_lastHoveredElement && !m_lastHoveredElement->isMouseOverHoverableArea(event.button.x, event.button.y))
          {
            // we're not hovering, so stop the tooltips
            m_uiManager.stopTooltip();
            // tell the previously hovered element we left it before resetting it
            m_lastHoveredElement->onMouseLeave(event);
            m_lastHoveredElement = nullptr;
            break;
          }
          // If we're over a UI element that has no click functionality, abort the event loop, so no clicks go through the UiElement.
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
          // definitely figure out a better way to do this, this was done for the Slider
          if (it->isMouseOver(event.button.x, event.button.y))
          {
            it->onMouseMove(event);
          }
        }
      }

      // game event handling
      if (m_panning)
      {
        Camera::cameraOffset.x -= event.motion.xrel;
        Camera::cameraOffset.y -= event.motion.yrel;

        if (Engine::instance().map != nullptr)
          Engine::instance().map->refresh();
      }
      else if (engine.map != nullptr)
      {
        mouseCoords = {event.button.x, event.button.y};
        clickCoords = convertScreenToIsoCoordinates(mouseCoords);

        if (highlightSelection && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
          if (terrainEditMode == TerrainEdit::RAISE)
          {
            // Add highlighting here
          }
          else if (terrainEditMode == TerrainEdit::LOWER)
          {
            // Add highlighting here
          }
          else if (!tileTypeEditMode.empty())
          {
            this->unHighlighNodes(engine);

            m_highlightedNodes = createBresenhamLine(m_clickDownCoords, clickCoords);

            for (size_t i = 0; i < m_highlightedNodes.size(); i++)
            {
              if (!engine.map->checkTileIDIsEmpty(m_highlightedNodes[i], tileTypeEditMode))
              {
                // already occupied tile, mark red
                engine.map->highlightNode(m_highlightedNodes[i], SpriteHighlightColor::RED);
              }
              else
              {
                // mark gray.
                engine.map->highlightNode(m_highlightedNodes[i], SpriteHighlightColor::GRAY);
              }
            }
          }
          else if (demolishMode)
          {
            // Add highlighting here
          }
        }
        else
        {
          this->unHighlighNodes(engine);
          // get Object Nodes in case it's a building bigger than 1x1 tile.
          m_highlightedObjectNodes = engine.map->getObjectCoords(clickCoords, tileTypeEditMode);
          if (m_highlightedObjectNodes.empty())
          {
            m_highlightedObjectNodes.push_back(clickCoords);
          }
          std::vector<Point> pointsToHighlight;
          if (highlightSelection)
          {
            for (auto coords : m_highlightedObjectNodes)
            {
              if (!engine.map->checkTileIDIsEmpty(coords, tileTypeEditMode))
              {
                Point origCornerPoint = engine.map->getNodeOrigCornerPoint(coords);
                engine.map->highlightNode(origCornerPoint, SpriteHighlightColor::RED);
                engine.map->highlightNode(coords, SpriteHighlightColor::RED);
                pointsToHighlight.push_back(origCornerPoint);
              }
              else
              {
                engine.map->highlightNode(coords, SpriteHighlightColor::GRAY);
              }
            }
            m_highlightedObjectNodes.insert(m_highlightedObjectNodes.end(), pointsToHighlight.begin(), pointsToHighlight.end());
          }
        }
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      m_skipLeftClick = false;
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElementsForEventHandling())
      {
        // only check visible elements
        if (it->isVisible() && it->onMouseButtonDown(event))
        {
          break;
        }
      }

      if (event.button.button == SDL_BUTTON_RIGHT)
      {
        m_panning = true;
      }
      else if (event.button.button == SDL_BUTTON_LEFT)
      {
        // game event handling
        mouseCoords = {event.button.x, event.button.y};
        clickCoords = convertScreenToIsoCoordinates(mouseCoords);

        if (isPointWithinMapBoundaries(clickCoords))
        {
          m_clickDownCoords = clickCoords;
        }
      }
      break;

    case SDL_MOUSEBUTTONUP:
    {
      std::vector<Point> bresenhamLineNodes = m_highlightedNodes;
      this->unHighlighNodes(engine);
      if (m_panning)
      {
        Camera::centerIsoCoordinates =
            convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
        m_panning = false;
      }
      // reset pinchCenterCoords when fingers are released
      pinchCenterCoords = {0, 0, 0, 0};
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElementsForEventHandling())
      {
        // only check visible elements
        if (it->isVisible())
        {
          // first, check if the element is a group and send the event
          if (it->onMouseButtonUp(event))
          {
            m_skipLeftClick = true;
            break;
          }
          // If we're over a UI element that has no click functionality, abort the event loop, so no clicks go through the UiElement.
          //Note: This is handled here because UIGroups have no dimensions, but are UiElements
          if (it->isMouseOver(event.button.x, event.button.y))
          {
            m_skipLeftClick = true;
          }
        }
      }
      // If we're over a ui element, don't handle game events
      if (m_skipLeftClick)
      {
        break;
      }

      // game event handling
      mouseCoords = {event.button.x, event.button.y};
      clickCoords = convertScreenToIsoCoordinates(mouseCoords);
      m_highlightedObjectNodes = engine.map->getObjectCoords(clickCoords, tileTypeEditMode);

      if (event.button.button == SDL_BUTTON_LEFT && isPointWithinMapBoundaries(m_highlightedObjectNodes))
      {
        if (m_tileInfoMode)
        {
          engine.map->getNodeInformation({clickCoords.x, clickCoords.y, 0, 0});
        }
        else if (terrainEditMode == TerrainEdit::RAISE)
        {
          engine.increaseHeight(clickCoords);
        }
        else if (terrainEditMode == TerrainEdit::LOWER)
        {
          engine.decreaseHeight(clickCoords);
        }
        else if (!tileTypeEditMode.empty())
        {
          if (bresenhamLineNodes.size() == 0)
          {
            //m_highlightedNodes.push_back(m_clickDownCoords);
            engine.setTileIDOfNode(m_highlightedObjectNodes.begin(), m_highlightedObjectNodes.end(), tileTypeEditMode, false);
          }
          else
          {
            engine.setTileIDOfNode(bresenhamLineNodes.begin(), bresenhamLineNodes.end(), tileTypeEditMode, true);
          }
        }
        else if (demolishMode)
        {
          engine.demolishNode(clickCoords);
        }
        else
        {
          LOG(LOG_INFO) << "CLICKED - Iso Coords: " << clickCoords.x << ", " << clickCoords.y;
        }
      }

      break;
    }
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

  for (auto &timer : timers)
  {
    if (timer)
      timer->checkTimeout();
  }

  for (std::vector<Timer *>::iterator it = timers.begin(); it != timers.end();)
  {
    if (!(*it)->isActive())
    {
      it = timers.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void EventManager::registerTimer(Timer *timer) { timers.push_back(timer); }
