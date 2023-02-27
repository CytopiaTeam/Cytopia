#include "EventManager.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/PointFunctions.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "basics/GameStates.hxx"
#include "common/enums.hxx"
#include "map/MapLayers.hxx"
#include "Map.hxx"
#include "Sprite.hxx"
#include "UIManager.hxx"
#include "WindowManager.hxx"
#include <MapFunctions.hxx>

#include "LOG.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"

#include "../game/ui/PauseMenu.hxx"

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

namespace detail
{
std::optional<PlacementMode> updatePlacementMode(Layer topMostActiveLayer)
{
  switch (topMostActiveLayer)
  {
  case Layer::BUILDINGS:
    return PlacementMode::SINGLE;
  case Layer::ROAD:
  case Layer::POWERLINES:
  case Layer::UNDERGROUND:
    return PlacementMode::LINE;
  case Layer::GROUND_DECORATION:
  case Layer::WATER:
  case Layer::ZONE:
    return PlacementMode::RECTANGLE;
  default:
    return {};
  }
}
} // namespace detail

using namespace detail;

void EventManager::unHighlightNodes()
{
  if (!m_isPuttingTile)
  {
    for (auto node : m_nodesToPlace)
    {
      MapFunctions::instance().unHighlightNode(node);
    }
    m_nodesToPlace.clear();
  }
  for (auto node : m_nodesToHighlight)
  {
    MapFunctions::instance().unHighlightNode(node);
  }
  m_nodesToHighlight.clear();
}

void EventManager::pickTileUnderCursor(Point mouseIsoCoords)
{
  Layer topMostActiveLayer;
  std::vector<MapNodeData> mapNodeData;
  const MapNode &node = MapFunctions::instance().getMapNode(mouseIsoCoords);

  topMostActiveLayer = node.getTopMostActiveLayer();
  // all layers are supported except terrain
  if (topMostActiveLayer == Layer::TERRAIN || topMostActiveLayer == Layer::NONE)
    return;
  // update placement mode
  if (const auto newPlacementMode = updatePlacementMode(topMostActiveLayer))
    GameStates::instance().placementMode = *newPlacementMode;
  mapNodeData = node.getMapNodeData();
  tileToPlace = mapNodeData[topMostActiveLayer].tileID;
  highlightSelection = true;
}

void EventManager::checkEvents(SDL_Event &event)
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("EventManager", "checkEvents", MP_BEIGE);
#endif
  // check for UI events first
  SDL_Point mouseScreenCoords;
  Point mouseIsoCoords{};
  auto &uiManager = UIManager::instance();

  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
      SignalMediator::instance().signalQuitGame.emit();
      break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
        if (!tileToPlace.empty())
        {
          uiManager.closeOpenMenus();
          tileToPlace.clear();
          highlightSelection = false;
        }
        else
        {
          uiManager.closeOpenMenus();

          if (uiManager.isAnyMenuOpen())
            uiManager.closeMenu();
          else
            uiManager.openMenu<PauseMenu>();
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
        uiManager.toggleDebugMenu();
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
      case SDLK_5:
        MapLayers::toggleLayer(Layer::ZONE);
        break;
      case SDLK_6:
        MapLayers::toggleLayer(Layer::GROUND_DECORATION);
        break;
      case SDLK_i:
        m_tileInfoMode = !m_tileInfoMode;
        break;
      case SDLK_h:
        // TODO: This is only temporary until the new UI is ready. Remove this afterwards
        GameStates::instance().drawUI = !GameStates::instance().drawUI;
        break;
      case SDLK_f:
        WindowManager::instance().toggleFullScreen();
        break;
      case SDLK_UP:
      case SDLK_w:
        if (MapFunctions::instance().getMap() &&
            Camera::instance().cameraOffset().y > -2 * Settings::instance().screenHeight * Camera::instance().zoomLevel())
        {
          Camera::instance().moveCamera(0, Settings::instance().screenHeight / 16);
        }
        break;
      case SDLK_LEFT:
      case SDLK_a:
        if (MapFunctions::instance().getMap() &&
            Camera::instance().cameraOffset().x > -0.25 * Settings::instance().screenWidth * Camera::instance().zoomLevel())
        {
          Camera::instance().moveCamera(Settings::instance().screenWidth / 16, 0);
        }
        break;
      case SDLK_DOWN:
      case SDLK_s:
        if (MapFunctions::instance().getMap() &&
            Camera::instance().cameraOffset().y < 1.25 * Settings::instance().screenHeight * Camera::instance().zoomLevel())
        {
          Camera::instance().moveCamera(0, -Settings::instance().screenHeight / 16);
        }
        break;
      case SDLK_RIGHT:
      case SDLK_d:
        if (MapFunctions::instance().getMap() &&
            Camera::instance().cameraOffset().x < 5 * Settings::instance().screenWidth * Camera::instance().zoomLevel())
        {
          // check if map exists to see, if we're ingame already.
          if (MapFunctions::instance().getMap())
          {
            Camera::instance().moveCamera(-Settings::instance().screenWidth / 16, 0);
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
                convertScreenToIsoCoordinates({static_cast<int>(event.mgesture.x * Settings::instance().screenWidth),
                                               static_cast<int>(event.mgesture.y * Settings::instance().screenHeight)});
          }
          Camera::instance().setPinchDistance(event.mgesture.dDist * 15.0F, m_pinchCenterCoords.x, m_pinchCenterCoords.y);
          break;
        }

        if (m_panning)
        {
          Camera::instance().moveCamera(static_cast<int>(Settings::instance().screenWidth * event.tfinger.dx),
                                        static_cast<int>(Settings::instance().screenHeight * event.tfinger.dy));
          break;
        }
      }
      break;
    case SDL_MOUSEMOTION:
      m_placementAllowed = false;
      m_cancelTileSelection = false;

      //  Game Event Handling
      if (MapFunctions::instance().getMap())
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
          Camera::instance().moveCamera(event.motion.xrel, event.motion.yrel);
        }
        // check if we should highlight tiles and if we're in placement mode
        if (highlightSelection)
        {
          mouseScreenCoords = {event.button.x, event.button.y};
          mouseIsoCoords = convertScreenToIsoCoordinates(mouseScreenCoords);

          // if it's a multi-node tile, get the origin corner point
          Point origCornerPoint =
              MapFunctions::instance().getNodeOrigCornerPoint(mouseIsoCoords, TileManager::instance().getTileLayer(tileToPlace));

          if (origCornerPoint == Point::INVALID())
          {
            origCornerPoint = mouseIsoCoords;
          }

          // canceling transparent buildings
          for (const auto &it : m_transparentBuildings)
          {
            if (it != Point::INVALID())
            {
              (MapFunctions::instance().getMapNode(it)).setNodeTransparency(0, Layer::BUILDINGS);
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
            m_nodesToHighlight = TileManager::instance().getTargetCoordsOfTileID(mouseIsoCoords, tileToPlace);
            // get all node coordinates the tile we'll place occupies

            if (m_nodesToHighlight.empty() && mouseIsoCoords.isWithinMapBoundaries())
            {
              m_nodesToHighlight.push_back(mouseIsoCoords);
            }
          }

          // if mouse is held down, we need to check for plamentmodes LINE and RECTANGLE
          if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
          {
            switch (GameStates::instance().placementMode)
            {
            case PlacementMode::SINGLE:
              m_nodesToPlace.push_back(mouseIsoCoords);
              break;
            case PlacementMode::LINE:
              m_nodesToPlace = PointFunctions::getLine(m_clickDownCoords, mouseIsoCoords);
              m_nodesToHighlight = m_nodesToPlace;
              break;
            case PlacementMode::STRAIGHT_LINE:
              m_nodesToPlace = PointFunctions::getStraightLine(m_clickDownCoords, mouseIsoCoords);
              m_nodesToHighlight = m_nodesToPlace;
              break;
            case PlacementMode::RECTANGLE:
              m_nodesToPlace = PointFunctions::getArea(m_clickDownCoords, mouseIsoCoords);
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
            Point currentOriginPoint = MapFunctions::instance().getNodeOrigCornerPoint(coords, layer);

            std::string currentTileID = MapFunctions::instance().getTileID(currentOriginPoint, layer);
            for (auto &foundNode : TileManager::instance().getTargetCoordsOfTileID(currentOriginPoint, currentTileID))
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

          m_placementAllowed = MapFunctions::instance().isPlacementOnAreaAllowed(m_nodesToHighlight, tileToPlace);

          // Finally highlight all the tiles we've found
          // Set highlighted tiles that can be placed and can't be placed different color
          for (const auto &highlitNode : m_nodesToHighlight)
          {
            if (!MapFunctions::instance().isPlacementOnNodeAllowed(highlitNode, tileToPlace) || demolishMode)
            {
              // mark red
              MapFunctions::instance().highlightNode(highlitNode, SpriteHighlightColor::RED);
            }
            else
            {
              // place allowed tile, mark gray
              MapFunctions::instance().highlightNode(highlitNode, SpriteHighlightColor::GRAY);
            }
            const Point &buildingCoordinates =
                MapFunctions::instance().findNodeInMap(convertIsoToScreenCoordinates(highlitNode), Layer::BUILDINGS);

            auto transparentBuildingIt =
                std::find(m_transparentBuildings.begin(), m_transparentBuildings.end(), buildingCoordinates);
            if ((transparentBuildingIt == m_transparentBuildings.end()) && (buildingCoordinates != Point::INVALID()))
            {
              const TileData *tileData = MapFunctions::instance().getMapNode(buildingCoordinates).getTileData(Layer::BUILDINGS);
              if (tileData && tileData->category != "Flora")
              {
                MapFunctions::instance().getMapNode(buildingCoordinates).setNodeTransparency(0.6f, Layer::BUILDINGS);
                m_transparentBuildings.push_back(buildingCoordinates);
              }
            }
          }
        }
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      m_placementAllowed = false;

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
        const std::vector targetObjectNodes = TileManager::instance().getTargetCoordsOfTileID(mouseIsoCoords, tileToPlace);

        //check if the coords for the click and for the occpuied tiles of the tileID we want to place are within map boundaries
        bool canPlaceTileID = false;
        if (mouseIsoCoords.isWithinMapBoundaries())
        {
          canPlaceTileID = true;
          for (auto coordinate : targetObjectNodes)
          {
            if (!coordinate.isWithinMapBoundaries())
            {
              canPlaceTileID = false;
              break;
            }
          }
        }

        if (canPlaceTileID)
        {
          m_clickDownCoords = mouseIsoCoords;
          m_placementAllowed = true;

          // Nodes to place are collected during the mouse move.
          // In case of multiple left clicks without moving the mouse, node to place will be the node of the mouse click.
          if (m_nodesToPlace.empty())
          {
            m_nodesToPlace.push_back(mouseIsoCoords);
          }
          m_isPuttingTile = true;
        }
      }
      break;

    case SDL_MOUSEBUTTONUP:
    {
      if (m_cancelTileSelection)
      {
        uiManager.closeOpenMenus();
        tileToPlace.clear();
        highlightSelection = false;
      }
      if (m_panning)
      {
        m_panning = false;
      }

      // reset pinchCenterCoords when fingers are released
      m_pinchCenterCoords = {0, 0, 0, 0};

      // game event handling
      mouseScreenCoords = {event.button.x, event.button.y};
      mouseIsoCoords = convertScreenToIsoCoordinates(mouseScreenCoords);
      // gather all nodes the objects that'll be placed is going to occupy.
      std::vector targetObjectNodes = TileManager::instance().getTargetCoordsOfTileID(mouseIsoCoords, tileToPlace);

      if (event.button.button == SDL_BUTTON_LEFT)
      {
        if (m_tileInfoMode)
        {
          MapFunctions::instance().getNodeInformation({mouseIsoCoords.x, mouseIsoCoords.y, 0, 0});
        }
        else if (terrainEditMode == TerrainEdit::RAISE)
        {
          MapFunctions::instance().changeHeight(mouseIsoCoords, true);
        }
        else if (terrainEditMode == TerrainEdit::LOWER)
        {
          MapFunctions::instance().changeHeight(mouseIsoCoords, false);
        }
        else if (terrainEditMode == TerrainEdit::LEVEL)
        {
          MapFunctions::instance().levelHeight(m_clickDownCoords, m_nodesToPlace);
        }
        else if (demolishMode)
        {
          MapFunctions::instance().demolishNode(m_nodesToHighlight, true);
        }
        // select the tile our cursor is over
        else if (!demolishMode && tileToPlace.empty())
        {
          pickTileUnderCursor(mouseIsoCoords);
          // pick the tile in mousemove, player will find mousedown
          break;
        }
        else if (!tileToPlace.empty() && m_placementAllowed)
        {
          if (!uiManager.isMouseHovered() && !MapFunctions::instance().setTileID(tileToPlace, m_nodesToPlace))
          {
            // If can't put picked tile here,
            // pick tile under cursor as the new picked tile
            // Thus the picker would always work without
            // having to right click or enter Esc(abort tile placing) first
            pickTileUnderCursor(mouseIsoCoords);
          }
        }
      }
      // when we're done, reset highlighting
      m_isPuttingTile = false;
      unHighlightNodes();

      if (highlightSelection)
      {
        m_nodesToHighlight.push_back(mouseIsoCoords);
        if (!uiManager.isMouseHovered() && !tileToPlace.empty() &&
            !MapFunctions::instance().setTileID(tileToPlace, mouseIsoCoords))
        {
          MapFunctions::instance().highlightNode(mouseIsoCoords, SpriteHighlightColor::RED);
        }
        else
        {
          MapFunctions::instance().highlightNode(mouseIsoCoords, SpriteHighlightColor::GRAY);
        }
      }

      break;
    }
    case SDL_MOUSEWHEEL:
      Camera::instance().changeZoomLevel(event.wheel.y > 0);
      break;

    default:
      break;
    }
  }
}
