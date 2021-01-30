#include "EventManager.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "basics/GameStates.hxx"
#include "common/enums.hxx"
#include "map/MapLayers.hxx"
#include "Map.hxx"
#include "Sprite.hxx"
#include "../../view/Window.hxx"
#include "LOG.hxx"

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

EventManager::~EventManager()
{
  debug_scope {
    LOG(LOG_DEBUG) << "Destroying Event Manager";
  }
}

void EventManager::unHighlightNodes()
{
  for (auto node : m_nodesToPlace)
  {
    Engine::instance().map->unHighlightNode(node);
  }
  for (auto node : m_nodesToHighlight)
  {
    Engine::instance().map->unHighlightNode(node);
  }
  m_nodesToHighlight.clear();
  m_nodesToPlace.clear();
}

void EventManager::checkEvents(SDL_Event &event, Engine &engine)
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("EventManager", "checkEvents", MP_BEIGE);
#endif
  // check for UI events first
  SDL_Point mouseScreenCoords;
  Point mouseIsoCoords{};

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
        if (!tileToPlace.empty())
        {
          m_uiManager.closeOpenMenus();
          tileToPlace.clear();
          highlightSelection = false;
        }
        else
        {
          m_uiManager.toggleGroupVisibility("PauseMenu");
        }
        break;

      case SDLK_0:
        break;
      case SDLK_LCTRL:
        GameStates::instance().demolishMode = DemolishMode::GROUND_DECORATION;
        GameStates::instance().rectangularRoads = true;
        break;
      case SDLK_LSHIFT:
        GameStates::instance().demolishMode = DemolishMode::DE_ZONE;
        if (GameStates::instance().placementMode == PlacementMode::LINE)
        {
          GameStates::instance().placementMode = PlacementMode::STRAIGHT_LINE;
        }
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
        MapLayers::toggleLayer(Layer::BLUEPRINT);
        break;
      case SDLK_4:
        MapLayers::toggleLayer(Layer::UNDERGROUND);
        break;
      case SDLK_i:
        m_tileInfoMode = !m_tileInfoMode;
        break;
      case SDLK_h:
        // TODO: This is only temporary until the new UI is ready. Remove this afterwards
        GameStates::instance().drawUI = !GameStates::instance().drawUI;
        break;
      case SDLK_f:
        engine.toggleFullScreen();
        break;
      case SDLK_UP:
      case SDLK_w:
        if (Camera::instance().cameraOffset().y > -2 * m_Window->getBounds().height()* Camera::instance().zoomLevel())
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::instance().moveCameraY(m_Window->getBounds().height() / 16);
            // set the center coordinates for scrolling
            Camera::instance().setCenterIsoCoordinates(
                convertScreenToIsoCoordinates({m_Window->getBounds().width() / 2, m_Window->getBounds().height() / 2}));
            Engine::instance().map->refresh();
          }
        }
        break;
      case SDLK_LEFT:
      case SDLK_a:
        if (Camera::instance().cameraOffset().x > -0.25 * m_Window->getBounds().width() * Camera::instance().zoomLevel())
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::instance().moveCameraX(m_Window->getBounds().width() / 16);
            // set the center coordinates for scrolling
            Camera::instance().setCenterIsoCoordinates(
                convertScreenToIsoCoordinates({m_Window->getBounds().width() / 2, m_Window->getBounds().height() / 2}));
            Engine::instance().map->refresh();
          }
        }
        break;
      case SDLK_DOWN:
      case SDLK_s:
        if (Camera::instance().cameraOffset().y < 1.25 * m_Window->getBounds().height() * Camera::instance().zoomLevel())
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::instance().moveCameraY(-m_Window->getBounds().height() / 16);
            // set the center coordinates for scrolling
            Camera::instance().setCenterIsoCoordinates(
                convertScreenToIsoCoordinates({m_Window->getBounds().width() / 2, m_Window->getBounds().height() / 2}));
            Engine::instance().map->refresh();
          }
        }
        break;
      case SDLK_RIGHT:
      case SDLK_d:
        if (Camera::instance().cameraOffset().x < 5 * m_Window->getBounds().width() * Camera::instance().zoomLevel())
        {
          // check if map exists to see, if we're ingame already.
          if (Engine::instance().map)
          {
            Camera::instance().moveCameraX(-m_Window->getBounds().width() / 16);
            // set the center coordinates for scrolling
            Camera::instance().setCenterIsoCoordinates(
                convertScreenToIsoCoordinates({m_Window->getBounds().width() / 2, m_Window->getBounds().height() / 2}));
            Engine::instance().map->refresh();
          }
        }
        break;

      default:
        break;
      }
      break;
    case SDL_KEYUP:
      switch (event.key.keysym.sym)
      {
      case SDLK_LCTRL:
        GameStates::instance().demolishMode = DemolishMode::DEFAULT;
        GameStates::instance().rectangularRoads = false;
        break;
      case SDLK_LSHIFT:
        GameStates::instance().demolishMode = DemolishMode::DEFAULT;
        if (GameStates::instance().placementMode == PlacementMode::STRAIGHT_LINE)
        {
          GameStates::instance().placementMode = PlacementMode::LINE;
        }
        break;

      default:
        break;
      }
    case SDL_MULTIGESTURE:
      if (event.mgesture.numFingers == 2)
      {
        m_panning = true;
        // check if we're pinching
        if (event.mgesture.dDist != 0)
        {
          // store pinchCenterCoords so they stay the same for all zoom levels
          if (m_pinchCenterCoords.x == 0 && m_pinchCenterCoords.y == 0)
          {
            m_pinchCenterCoords =
                convertScreenToIsoCoordinates({static_cast<int>(event.mgesture.x * m_Window->getBounds().width()),
                                               static_cast<int>(event.mgesture.y * m_Window->getBounds().height())});
          }
          Camera::instance().setPinchDistance(event.mgesture.dDist * 15.0F, m_pinchCenterCoords.x, m_pinchCenterCoords.y);
          m_skipLeftClick = true;
          break;
        }

        if (m_panning)
        {
          Camera::instance().moveCameraX(static_cast<int>(m_Window->getBounds().width() * event.tfinger.dx));
          Camera::instance().moveCameraY(static_cast<int>(m_Window->getBounds().height() * event.tfinger.dy));
          Camera::instance().setCenterIsoCoordinates(
              convertScreenToIsoCoordinates({m_Window->getBounds().width() / 2, m_Window->getBounds().height() / 2}));
          Engine::instance().map->refresh();
          m_skipLeftClick = true;
          break;
        }
        m_skipLeftClick = true;
      }
      break;
    case SDL_MOUSEMOTION:
      m_placementAllowed = false;
      m_cancelTileSelection = false;
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

      //  Game Event Handling
      if (engine.map)
      {
        // clear highlighting
        unHighlightNodes();

        // if we're panning, move the camera and break
        if (m_panning)
        {
          if ((event.motion.xrel == 0) && (event.motion.yrel == 0))
          {
            return;
          }
          Camera::instance().moveCameraX(event.motion.xrel);
          Camera::instance().moveCameraY(event.motion.yrel);

          Engine::instance().map->refresh();
        }
        // check if we should highlight tiles and if we're in placement mode
        if (highlightSelection)
        {
          mouseScreenCoords = {event.button.x, event.button.y};
          const Point terrainCoordinates = engine.map->findNodeInMap(mouseScreenCoords, Layer::TERRAIN);
          const Point buildingCoordinates = engine.map->findNodeInMap(mouseScreenCoords, Layer::BUILDINGS);
          mouseIsoCoords = convertScreenToIsoCoordinates(mouseScreenCoords);

          // if it's a multi-node tile, get the origin corner point
          Point origCornerPoint =
              engine.map->getNodeOrigCornerPoint(mouseIsoCoords, TileManager::instance().getTileLayer(tileToPlace));

          if (origCornerPoint == Point::INVALID())
          {
            origCornerPoint = mouseIsoCoords;
          }

          // canceling transparent buildings
          for (const auto &it : m_transparentBuildings)
          {
            if (it != Point::INVALID())
            {
              (engine.map->getMapNode(it))->setNodeTransparency(0, Layer::BUILDINGS);
            }
          }
          m_transparentBuildings.clear();

          // if there's no tileToPlace use the current mouse coordinates
          if (tileToPlace.empty())
          {
            m_nodesToHighlight.push_back(mouseIsoCoords);
          }
          else
          {
            // get all node coordinates the tile we'll place occupies
            for (auto &node : engine.map->getObjectCoords(mouseIsoCoords, tileToPlace))
            {
              // if we don't geta correct coordinate, fall back to the click coordinates
              if (node == Point::INVALID() && isPointWithinMapBoundaries(mouseIsoCoords))
              {
                m_nodesToHighlight.push_back(mouseIsoCoords);
              }
              else if (isPointWithinMapBoundaries(node))
              {
                m_nodesToHighlight.push_back(node);
              }
            }
          }

          // if mouse is held down, we need to check for plamentmodes LINE and RECTANGLE
          if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
          {
            switch (GameStates::instance().placementMode)
            {
            case PlacementMode::SINGLE:
              m_nodesToPlace.push_back(mouseIsoCoords);
              break;
            case PlacementMode::LINE:
              m_nodesToPlace = createBresenhamLine(m_clickDownCoords, mouseIsoCoords);
              m_nodesToHighlight = m_nodesToPlace;
              break;
            case PlacementMode::STRAIGHT_LINE:
              m_nodesToPlace = getRectangularLineSelectionNodes(m_clickDownCoords, mouseIsoCoords);
              m_nodesToHighlight = m_nodesToPlace;
              break;
            case PlacementMode::RECTANGLE:
              m_nodesToPlace = getRectangleSelectionNodes(m_clickDownCoords, mouseIsoCoords);
              m_nodesToHighlight = m_nodesToPlace;
              break;
            }
          }

          // if we haven't any nodes to place yet, use the mouse coordinates
          if (m_nodesToPlace.empty())
          {
            m_nodesToPlace.push_back(mouseIsoCoords);
          }
          m_placementAllowed = false;
          std::vector<Point> nodesToAdd;
          TileData *tileToPlaceData = TileManager::instance().getTileData(tileToPlace);

          // if we touch a bigger than 1x1 tile also add all nodes of the building to highlight.
          for (const auto &coords : m_nodesToHighlight)
          {
            // If we place a ground decoration tile, we must add all tiles of bigger than 1x1 buildings from the Layer BUILDINGS
            Layer layer;
            if (demolishMode || (tileToPlaceData && tileToPlaceData->tileType == +TileType::GROUNDDECORATION))
            {
              layer = Layer::BUILDINGS;
            }
            else
            {
              layer = TileManager::instance().getTileLayer(tileToPlace);
            }
            Point currentOriginPoint = engine.map->getNodeOrigCornerPoint(coords, layer);

            std::string currentTileID = engine.map->getTileID(currentOriginPoint, layer);
            for (auto &foundNode : engine.map->getObjectCoords(currentOriginPoint, currentTileID))
            {
              // only add the node if it's unique
              if (std::find(m_nodesToHighlight.begin(), m_nodesToHighlight.end(), foundNode) == m_nodesToHighlight.end())
              {
                nodesToAdd.push_back(foundNode);
              }
            }
          }
          // add the nodes we've found
          m_nodesToHighlight.insert(m_nodesToHighlight.end(), nodesToAdd.begin(), nodesToAdd.end());

          // for ground decoration, place all ground decoration files beneath the building
          if (tileToPlaceData && tileToPlaceData->tileType == +TileType::GROUNDDECORATION)
          {
            m_nodesToPlace = m_nodesToHighlight;
          }

          // we need to check if placement is allowed and set a bool to color ALL the highlighted tiles and not just those who can't be placed
          for (const auto &highlitNode : m_nodesToHighlight)
          {
            if (!engine.map->isPlacementOnNodeAllowed(highlitNode, tileToPlace) || demolishMode)
            {
              // already occupied tile, mark red
              m_placementAllowed = false;
              break;
            }

            // mark gray.
            m_placementAllowed = true;
          }
          // finally highlight all the tiles we've found
          for (const auto &highlitNode : m_nodesToHighlight)
          {
            if (m_placementAllowed)
            {
              engine.map->highlightNode(highlitNode, SpriteHighlightColor::GRAY);
            }
            else
            {
              engine.map->highlightNode(highlitNode, SpriteHighlightColor::RED);
            }
            const Point &buildingCoordinates =
                engine.map->findNodeInMap(convertIsoToScreenCoordinates(highlitNode), Layer::BUILDINGS);

            auto transparentBuildingIt =
                std::find(m_transparentBuildings.begin(), m_transparentBuildings.end(), buildingCoordinates);
            if ((transparentBuildingIt == m_transparentBuildings.end()) && (buildingCoordinates != Point::INVALID()))
            {
              const TileData *tileData = engine.map->getMapNode(buildingCoordinates)->getTileData(Layer::BUILDINGS);
              if (tileData && tileData->category != "Flora")
              {
                engine.map->getMapNode(buildingCoordinates)->setNodeTransparency(0.6f, Layer::BUILDINGS);
                m_transparentBuildings.push_back(buildingCoordinates);
              }
            }
          }
        }
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      m_placementAllowed = false;
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
        m_cancelTileSelection = true;
      }
      else if (event.button.button == SDL_BUTTON_LEFT)
      {
        // game event handling
        mouseScreenCoords = {event.button.x, event.button.y};
        mouseIsoCoords = convertScreenToIsoCoordinates(mouseScreenCoords);
        const std::vector targetObjectNodes = engine.map->getObjectCoords(mouseIsoCoords, tileToPlace);

        if (isPointWithinMapBoundaries(mouseIsoCoords) && isPointWithinMapBoundaries(targetObjectNodes))
        {
          m_clickDownCoords = mouseIsoCoords;
          m_placementAllowed = true;

          // Nodes to place are collected during the mouse move.
          // In case of multiple left clicks without moving the mouse, node to place will be the node of the mouse click.
          if (m_nodesToPlace.empty())
          {
            m_nodesToPlace.push_back(mouseIsoCoords);
          }
        }
      }
      break;

    case SDL_MOUSEBUTTONUP:
    {
      if (m_cancelTileSelection)
      {
        m_uiManager.closeOpenMenus();
        tileToPlace.clear();
        highlightSelection = false;
      }
      if (m_panning)
      {
        Camera::instance().setCenterIsoCoordinates(
            convertScreenToIsoCoordinates({m_Window->getBounds().width() / 2, m_Window->getBounds().height() / 2}));
        m_panning = false;
      }

      // reset pinchCenterCoords when fingers are released
      m_pinchCenterCoords = {0, 0, 0, 0};
      // check for UI events first
      for (const auto &it : m_uiManager.getAllUiElementsForEventHandling())
      {
        // only check visible elements
        if (it->isVisible() && event.button.button == SDL_BUTTON_LEFT)
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
      mouseScreenCoords = {event.button.x, event.button.y};
      mouseIsoCoords = convertScreenToIsoCoordinates(mouseScreenCoords);
      // gather all nodes the objects that'll be placed is going to occupy.
      std::vector targetObjectNodes = engine.map->getObjectCoords(mouseIsoCoords, tileToPlace);

      if (event.button.button == SDL_BUTTON_LEFT)
      {
        if (m_tileInfoMode)
        {
          engine.map->getNodeInformation({mouseIsoCoords.x, mouseIsoCoords.y, 0, 0});
        }
        else if (terrainEditMode == TerrainEdit::RAISE)
        {
          engine.increaseHeight(mouseIsoCoords);
        }
        else if (terrainEditMode == TerrainEdit::LOWER)
        {
          engine.decreaseHeight(mouseIsoCoords);
        }
        else if (!tileToPlace.empty() && m_placementAllowed)
        {
          // if targetObject.size > 1 it is a tile bigger than 1x1
          if (targetObjectNodes.size() > 1 && isPointWithinMapBoundaries(targetObjectNodes))
          {
            // instead of using "nodesToPlace" which would be the origin-corner coordinate, we need to pass ALL occupied nodes for now.
            engine.setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), tileToPlace, false);
          }
          else
          {
            engine.setTileIDOfNode(m_nodesToPlace.begin(), m_nodesToPlace.end(), tileToPlace, true);
          }
        }
        else if (demolishMode)
        {
          engine.map->demolishNode(m_nodesToHighlight, true);
        }
      }
      // when we're done, reset highlighting
      unHighlightNodes();

      if (highlightSelection)
      {
        m_nodesToHighlight.push_back(mouseIsoCoords);
        if (!tileToPlace.empty() && !engine.map->isPlacementOnNodeAllowed(mouseIsoCoords, tileToPlace))
        {
          engine.map->highlightNode(mouseIsoCoords, SpriteHighlightColor::RED);
        }
        else
        {
          engine.map->highlightNode(mouseIsoCoords, SpriteHighlightColor::GRAY);
        }
      }

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

    default:
      break;
    }
  }

  for (auto &timer : m_timers)
  {
    if (timer)
      timer->checkTimeout();
  }

  for (std::vector<Timer *>::iterator it = m_timers.begin(); it != m_timers.end();)
  {
    if (!(*it)->isActive())
    {
      it = m_timers.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void EventManager::registerTimer(Timer *timer) { m_timers.push_back(timer); }

void EventManager::setWindow(Window* window) {
  m_Window = window;
}
