#include "Map.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "LOG.hxx"
#include "common/Constants.hxx"
#include "ResourcesManager.hxx"
#include "map/MapLayers.hxx"
#include "common/JsonSerialization.hxx"
#include "Filesystem.hxx"
#include "../services/Randomizer.hxx"

#include "json.hxx"

#include <sstream>
#include <string>
#include <set>
#include <queue>

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

using json = nlohmann::json;

NeighborNodesPosition operator++(NeighborNodesPosition &nn, int)
{
  NeighborNodesPosition res = nn;

  switch (nn)
  {
  case NeighborNodesPosition::BOTTOM_LEFT:
    nn = NeighborNodesPosition::LEFT;
    break;
  case NeighborNodesPosition::LEFT:
    nn = NeighborNodesPosition::TOP_LEFT;
    break;
  case NeighborNodesPosition::TOP_LEFT:
    nn = NeighborNodesPosition::BOTTOM;
    break;
  case NeighborNodesPosition::BOTTOM:
    nn = NeighborNodesPosition::CENTER;
    break;
  case NeighborNodesPosition::CENTER:
    nn = NeighborNodesPosition::TOP;
    break;
  case NeighborNodesPosition::TOP:
    nn = NeighborNodesPosition::BOTTOM_RIGHT;
    break;
  case NeighborNodesPosition::BOTTOM_RIGHT:
    nn = NeighborNodesPosition::RIGHT;
    break;
  case NeighborNodesPosition::RIGHT:
    nn = NeighborNodesPosition::TOP_RIGHT;
    break;
  case NeighborNodesPosition::TOP_RIGHT:
    nn = NeighborNodesPosition::BOTTOM_LEFT;
    break;
  default:
    assert(false);
    break;
  }

  return res;
}

void Map::getNodeInformation(const Point &isoCoordinates) const
{
  const MapNode &mapNode = mapNodes[isoCoordinates.toIndex()];
  const MapNodeData &mapNodeData = mapNode.getActiveMapNodeData();
  const TileData *tileData = mapNodeData.tileData;
  LOG(LOG_INFO) << "===== TILE at " << isoCoordinates.x << ", " << isoCoordinates.y << ", " << mapNode.getCoordinates().height
                << "=====";
  LOG(LOG_INFO) << "[Layer: TERRAIN] ID: " << mapNode.getMapNodeDataForLayer(Layer::TERRAIN).tileID;
  LOG(LOG_INFO) << "[Layer: WATER] ID: " << mapNode.getMapNodeDataForLayer(Layer::WATER).tileID;
  LOG(LOG_INFO) << "[Layer: BUILDINGS] ID: " << mapNode.getMapNodeDataForLayer(Layer::BUILDINGS).tileID;
  LOG(LOG_INFO) << "Category: " << tileData->category;
  LOG(LOG_INFO) << "FileName: " << tileData->tiles.fileName;
  LOG(LOG_INFO) << "PickRandomTile: " << tileData->tiles.pickRandomTile;
  LOG(LOG_INFO) << "TileMap: " << mapNodeData.tileMap;
  LOG(LOG_INFO) << "TileIndex: " << mapNodeData.tileIndex;
}

Map::Map(int columns, int rows, const bool generateTerrain)
    : pMapNodesVisible(new Sprite *[columns * rows]), m_columns(columns), m_rows(rows)
{
  // TODO move Random Engine out of map
  randomEngine.seed();
  MapLayers::enableLayers({TERRAIN, BUILDINGS, WATER, GROUND_DECORATION, ZONE, ROAD, POWERLINES, FLORA});

  if (generateTerrain)
  {
    m_terrainGen.generateTerrain(mapNodes, mapNodesInDrawingOrder);
  }

  updateAllNodes();
}

Map::~Map() { delete[] pMapNodesVisible; }

std::vector<NeighborNode> Map::getNeighborNodes(const Point &isoCoordinates, const bool includeCentralNode)
{
  std::vector<NeighborNode> neighbors;

  for (auto it : PointFunctions::getNeighbors(isoCoordinates, includeCentralNode))
  {
    neighbors.push_back({&getMapNode(it), PointFunctions::getNeighborPositionToOrigin(it, isoCoordinates)});
  }

  return neighbors;
}

bool Map::updateHeight(Point coordinate, const bool elevate)
{
  if (getMapNode(coordinate).changeHeight(elevate))
  {
    for (const auto neighbor : PointFunctions::getNeighbors(coordinate, false))
    {
      if (getMapNode(neighbor).isLayerOccupied(Layer::ZONE))
      {
        getMapNode(neighbor).demolishLayer(Layer::ZONE);
      }
    }

    return true;
  }

  return false;
}

void Map::changeHeight(const Point &isoCoordinates, const bool elevate)
{
  std::vector<Point> nodesToUpdate{isoCoordinates};
  std::vector<Point> neighorCoordinates = PointFunctions::getNeighbors(isoCoordinates, true);

  if (updateHeight(isoCoordinates, elevate))
  {
    // If lowering node height, than all nodes around should be lowered to be on same height with the central one.
    if (!elevate)
    {
      const int centerHeight = getMapNode(isoCoordinates).getCoordinates().height;

      for (Point &neighborCoord : neighorCoordinates)
      {
        MapNode &neighborNode = getMapNode(neighborCoord);
        if (centerHeight < neighborNode.getCoordinates().height)
        {
          neighborNode.changeHeight(false);
          nodesToUpdate.push_back(neighborNode.getCoordinates());
        }
      }
    }
    demolishNode(neighorCoordinates);
    updateNodeNeighbors(nodesToUpdate);
  }
}

void Map::increaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, true); }

void Map::decreaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, false); }

void Map::updateNodeNeighbors(std::vector<Point> nodes)
{
  // those bitmask combinations require the tile to be elevated.
  constexpr unsigned char elevateTileComb[] = {
      NeighborNodesPosition::TOP | NeighborNodesPosition::BOTTOM,
      NeighborNodesPosition::LEFT | NeighborNodesPosition::RIGHT,
      NeighborNodesPosition::TOP_LEFT | NeighborNodesPosition::RIGHT | NeighborNodesPosition::BOTTOM,
      NeighborNodesPosition::TOP_RIGHT | NeighborNodesPosition::LEFT | NeighborNodesPosition::BOTTOM,
      NeighborNodesPosition::BOTTOM_LEFT | NeighborNodesPosition::RIGHT | NeighborNodesPosition::TOP,
      NeighborNodesPosition::BOTTOM_RIGHT | NeighborNodesPosition::LEFT | NeighborNodesPosition::TOP};

  std::vector<Point> nodesToBeUpdated;
  std::map<int, std::vector<Point>> nodeCache;
  std::queue<Point> updatedNodes;
  std::vector<Point> nodesToElevate;
  std::vector<Point> nodesToDemolish;

  for (auto currentNode : nodes)
  {
    updatedNodes.push(currentNode);

    while (!updatedNodes.empty() || !nodesToElevate.empty())
    {
      while (!updatedNodes.empty())
      {
        const Point pHeighChangedNode = getMapNode(updatedNodes.front()).getCoordinates();
        updatedNodes.pop();
        const int tileHeight = pHeighChangedNode.height;

        if (nodeCache.count(pHeighChangedNode.toIndex()) == 0)
        {
          nodeCache[pHeighChangedNode.toIndex()] = PointFunctions::getNeighbors(pHeighChangedNode, false);
        }

        if (std::find(nodesToElevate.begin(), nodesToElevate.end(), pHeighChangedNode) == nodesToElevate.end())
        {
          nodesToElevate.push_back(pHeighChangedNode);
        }

        for (auto neighbor : nodeCache[pHeighChangedNode.toIndex()])
        {
          // get real coords that include height
          const Point neighborCoords = getMapNode(neighbor).getCoordinates();
          const int heightDiff = tileHeight - neighborCoords.height;

          if (nodeCache.count(neighborCoords.toIndex()) == 0)
          {
            nodeCache[neighborCoords.toIndex()] = PointFunctions::getNeighbors(neighborCoords, false);
          }

          if (std::find(nodesToElevate.begin(), nodesToElevate.end(), neighborCoords) == nodesToElevate.end())
          {
            nodesToElevate.push_back(neighborCoords);
          }

          if (std::abs(heightDiff) > 1)
          {
            updatedNodes.push(getMapNode(neighborCoords).getCoordinates());
            updateHeight(neighborCoords, (heightDiff > 1) ? true : false);
          }
        }
      }

      while (updatedNodes.empty() && !nodesToElevate.empty())
      {
        Point nodeToElevate = nodesToElevate.back();
        nodesToBeUpdated.push_back(nodeToElevate);
        nodesToElevate.pop_back();

        if (nodeCache.count(nodeToElevate.toIndex()) == 0)
        {
          nodeCache[nodeToElevate.toIndex()] = PointFunctions::getNeighbors(nodeToElevate, false);
        }
        const unsigned char elevationBitmask = getElevatedNeighborBitmask(nodeToElevate);

        if (elevationBitmask != getMapNode(nodeToElevate).getElevationBitmask())
        {
          nodesToDemolish.push_back(nodeToElevate);
          getMapNode(nodeToElevate).setElevationBitMask(elevationBitmask);
        }

        for (const auto &elBitMask : elevateTileComb)
        {
          if ((elevationBitmask & elBitMask) == elBitMask)
          {
            updateHeight(nodeToElevate, true);
            updatedNodes.push(getMapNode(nodeToElevate).getCoordinates());
            break;
          }
        }
      }
    }
  }

  if (!nodesToDemolish.empty())
  {
    demolishNode(nodesToDemolish);
  }

  for (Point node : nodesToBeUpdated)
  {
    getMapNode(node).setAutotileBitMask(calculateAutotileBitmask(node));
  }

  for (Point node : nodesToBeUpdated)
  {
    getMapNode(node).updateTexture();
  }
}

void Map::updateAllNodes()
{
  std::vector<Point> allCoords(mapNodes.size());
  std::transform(mapNodes.begin(), mapNodes.end(), allCoords.begin(), [](MapNode &mn) { return mn.getCoordinates(); });
  updateNodeNeighbors(allCoords);
}

bool Map::isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const
{
  return mapNodes[isoCoordinates.toIndex()].isPlacementAllowed(tileID);
}

bool Map::isPlacementOnAreaAllowed(const std::vector<Point> targetCoordinates, const std::string &tileID) const
{
  // This function can be divided into two policies:
  // Whether we need all nodes in the area to be placed or not
  bool should_all_nodes_placed = true;
  bool area_placement_allowed = true;
  bool tile_placement_allowed = true;

  TileData *tileData = TileManager::instance().getTileData(tileID);
  const Layer layer = TileManager::instance().getTileLayer(tileID);
  // Zone layer can be placed on part of tile in the area.
  // Some other layers can also have this feature, such as water, flora.
  if (layer == Layer::ZONE)
  {
    should_all_nodes_placed = false;
  } else {
    should_all_nodes_placed = true;
  }
  area_placement_allowed = should_all_nodes_placed;

  for (auto coord : targetCoordinates)
  {
    tile_placement_allowed = isPlacementOnNodeAllowed(coord, tileID);

    if (tile_placement_allowed && !should_all_nodes_placed)
    {
      area_placement_allowed = true;
      break;
    }
    if (!tile_placement_allowed && should_all_nodes_placed)
    {
      area_placement_allowed = false;
      break;
    }
  }

  return area_placement_allowed;
}

unsigned char Map::getElevatedNeighborBitmask(Point centerCoordinates)
{
  unsigned char bitmask = 0;
  const auto centralNodeHeight = getMapNode(centerCoordinates).getCoordinates().height;

  for (const auto &neighborCoordinates : PointFunctions::getNeighbors(centerCoordinates, false))
  {
    if (getMapNode(neighborCoordinates).getCoordinates().height > centralNodeHeight)
    {
      bitmask |= PointFunctions::getNeighborPositionToOrigin(neighborCoordinates, centerCoordinates);
    }
  }

  return bitmask;
}

Point Map::getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer)
{
  if ((layer != Layer::NONE) && isoCoordinates.isWithinMapBoundaries())
  {
    return getMapNode(isoCoordinates).getOrigCornerPoint(layer);
  }

  return Point::INVALID();
}

std::vector<uint8_t> Map::calculateAutotileBitmask(Point coordinate)
{
  std::vector<uint8_t> tileOrientationBitmask(LAYERS_COUNT, 0);

  for (auto currentLayer : allLayersOrdered)
  {
    auto pCurrentTileData = getMapNode(coordinate).getMapNodeDataForLayer(currentLayer).tileData;

    if (pCurrentTileData)
    {
      if (pCurrentTileData->tileType == +TileType::TERRAIN)
      {
        for (const auto &neighbor : getNeighborNodes(coordinate, false))
        {
          const auto pTileData = neighbor.pNode->getMapNodeDataForLayer(Layer::WATER).tileData;

          if (pTileData && pTileData->tileType == +TileType::WATER)
          {
            tileOrientationBitmask[currentLayer] |= neighbor.position;
          }
        }
      }

      // only auto-tile categories that can be tiled.
      const std::string &nodeTileId = getMapNode(coordinate).getMapNodeDataForLayer(currentLayer).tileID;
      if (TileManager::instance().isTileIDAutoTile(nodeTileId))
      {
        for (const auto &neighbor : getNeighborNodes(coordinate, false))
        {
          const MapNodeData &nodeData = neighbor.pNode->getMapNodeDataForLayer(currentLayer);

          if (nodeData.tileData && ((nodeData.tileID == nodeTileId) || (pCurrentTileData->tileType == +TileType::ROAD)))
          {
            tileOrientationBitmask[currentLayer] |= neighbor.position;
          }
        }
      }
    }
  }
  return tileOrientationBitmask;
}

void Map::renderMap() const
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Render Map", MP_YELLOW);
#endif

  for (int i = 0; i < m_visibleNodesCount; ++i)
  {
    pMapNodesVisible[i]->render();
  }
}

void Map::refresh()
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Refresh Map", MP_YELLOW);
#endif

  calculateVisibleMap();

  for (int i = 0; i < m_visibleNodesCount; ++i)
  {
    pMapNodesVisible[i]->refresh();
  }
}

//TODO: move it out from the map
SDL_Color Map::getColorOfPixelInSurface(SDL_Surface *surface, int x, int y) const
{
  SDL_Color Color{0, 0, 0, SDL_ALPHA_TRANSPARENT};

  if (surface)
  {
    const int bpp = surface->format->BytesPerPixel;
    Uint8 *p = &static_cast<Uint8 *>(surface->pixels)[y * surface->pitch + x * bpp];
    const Uint32 pixel = *reinterpret_cast<Uint32 *>(p);

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }

  return Color;
}

Point Map::findNodeInMap(const SDL_Point &screenCoordinates, const Layer &layer)
{
  // calculate clicked column (x coordinate) without height taken into account.
  const Point calculatedIsoCoords = calculateIsoCoordinates(screenCoordinates);
  int isoX = calculatedIsoCoords.x;
  int isoY = calculatedIsoCoords.y;

  // adjust calculated values that are outside of the map (which is legit, but they need to get pushed down)
  // only y can be out of bounds on our map
  if (isoY >= Settings::instance().mapSize)
  {
    int diff = isoY - Settings::instance().mapSize; // the diff to reset the value to the edge of the map
    // travel the column downwards.
    isoX += diff;
    isoY -= diff;
  }

  // Transverse a column form from calculated coordinates to the bottom of the map.
  // It is necessary to include 2 neighbor nodes from both sides.
  // Try to find map node in Z order.
  // Node with the highest Z order has highest X and lowest Y coordinate, so search will be conducted in that order.
  const int neighborReach = 2;
  const int mapSize = Settings::instance().mapSize;

  // Max X will reach end of the map or Y will reach 0.
  const int xMax = std::min(isoX + neighborReach + isoY, mapSize - 1);
  // Min X will reach 0 or x -2 neighbor node.
  const int xMin = std::max(isoX - neighborReach, 0);

  for (int x = xMax; x >= xMin; --x)
  {
    const int diff = x - isoX;
    const int yMiddlePoint = isoY - diff;

    // Move y up and down 2 neighbors.
    for (int y = std::max(yMiddlePoint - neighborReach, 0); (y <= yMiddlePoint + neighborReach) && (y < mapSize); ++y)
    {
      //get all coordinates for node at x,y
      Point coordinate = getMapNode(Point(x, y)).getCoordinates();

      if (isClickWithinTile(screenCoordinates, coordinate, layer))
      {
        return coordinate;
      }
    }
  }

  return Point{-1, -1, 0, 0};
}

void Map::demolishNode(const std::vector<Point> &isoCoordinates, bool updateNeighboringTiles, Layer layer)
{
  std::vector<Point> nodesToDemolish;

  for (Point currentCoordinate : isoCoordinates)
  {
    if (currentCoordinate.isWithinMapBoundaries())
    {
      MapNode &node = getMapNode(currentCoordinate);

      // Check for multi-node buildings first. Those are on the buildings layer, even if we want to demolish another layer than Buildings.
      // In case we add more Layers that support Multi-node, add a for loop here
      // If demolishNode is called for layer GROUNDECORATION, we'll still need to gather all nodes from the multi-node building to delete the decoration under the entire building
      auto pNodeTileData = node.getMapNodeDataForLayer(Layer::BUILDINGS).tileData;

      if (pNodeTileData && ((pNodeTileData->RequiredTiles.height > 1) || (pNodeTileData->RequiredTiles.width > 1)))
      {
        const Point origCornerPoint = node.getOrigCornerPoint(Layer::BUILDINGS);

        if (origCornerPoint.isWithinMapBoundaries())
        {
          const std::string &tileID = getMapNode(origCornerPoint).getTileID(Layer::BUILDINGS);

          // get all the occupied nodes and demolish them
          for (auto buildingCoords : TileManager::instance().getTargetCoordsOfTileID(origCornerPoint, tileID))
          {
            nodesToDemolish.push_back(buildingCoords);
          }
        }
      }
      // add the current coorindate
      nodesToDemolish.push_back(currentCoordinate);
    }
  }

  std::vector<Point> updateNodes;
  for (auto nodeCoordinate : nodesToDemolish)
  {
    getMapNode(nodeCoordinate).demolishNode(layer);
    SignalMediator::instance().signalDemolish.emit(&getMapNode(nodeCoordinate));
    // TODO: Play sound effect here
    if (updateNeighboringTiles)
    {
      updateNodes.push_back(nodeCoordinate);
    }
  }

  if (!updateNodes.empty())
  {
    updateNodeNeighbors(updateNodes);
  }
}

bool Map::isClickWithinTile(const SDL_Point &screenCoordinates, Point isoCoordinate, const Layer &layer = Layer::NONE) const
{
  if (!isoCoordinate.isWithinMapBoundaries())
  {
    return false;
  }

  auto &node = mapNodes[isoCoordinate.toIndex()];
  auto pSprite = node.getSprite();
  std::vector<Layer> layersToGoOver;

  // Layers ordered for hitcheck
  if (layer == Layer::NONE)
  {
    Layer layers[] = {Layer::TERRAIN, Layer::WATER, Layer::UNDERGROUND, Layer::BLUEPRINT};
    layersToGoOver.insert(layersToGoOver.begin(), std::begin(layers), std::end(layers));
  }
  else
  {
    layersToGoOver.push_back(layer);
  }

  for (auto &curLayer : layersToGoOver)
  {
    if (!MapLayers::isLayerActive(curLayer))
    {
      continue;
    }

    SDL_Rect spriteRect = pSprite->getDestRect(curLayer);
    SDL_Rect clipRect = pSprite->getClipRect(curLayer);

    if (curLayer == Layer::TERRAIN)
    {
      clipRect.h += 1; //HACK: We need to increase clipRect height by one pixel to match the drawRect. Rounding issue?
    }

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      std::string tileID = node.getMapNodeDataForLayer(curLayer).tileID;
      assert(!tileID.empty());

      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      const int pixelX = static_cast<int>((screenCoordinates.x - spriteRect.x) / Camera::instance().zoomLevel()) + clipRect.x;
      const int pixelY = static_cast<int>((screenCoordinates.y - spriteRect.y) / Camera::instance().zoomLevel()) + clipRect.y;

      if ((curLayer == Layer::TERRAIN) && (node.getMapNodeDataForLayer(Layer::TERRAIN).tileMap == TileMap::SHORE))
      {
        tileID.append("_shore");
      }

      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(tileID), pixelX, pixelY).a !=
          SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }

  // Nothing found
  return false;
}

void Map::highlightNode(const Point &isoCoordinates, const SpriteRGBColor &rgbColor)
{
  if (isoCoordinates.isWithinMapBoundaries())
  {
    const auto pSprite = getMapNode(isoCoordinates).getSprite();
    pSprite->highlightColor = rgbColor;
    pSprite->highlightSprite = true;
  }
}

std::string Map::getTileID(const Point &isoCoordinates, Layer layer)
{
  return (isoCoordinates.isWithinMapBoundaries()) ? getMapNode(isoCoordinates).getTileID(layer) : "";
}

void Map::unHighlightNode(const Point &isoCoordinates)
{
  if (isoCoordinates.isWithinMapBoundaries())
  {
    getMapNode(isoCoordinates).getSprite()->highlightSprite = false;
  }
}

void Map::saveMapToFile(const std::string &fileName)
{
  // make sure savegame dir exists
  fs::createDirectory(CYTOPIA_SAVEGAME_DIR);

  //create savegame json string
  const json j =
      json{{"Savegame version", SAVEGAME_VERSION}, {"columns", this->m_columns}, {"rows", this->m_rows}, {"mapNode", mapNodes}};

#ifdef DEBUG
  // Write uncompressed savegame for easier debugging
  fs::writeStringToFile(fileName + ".txt", j.dump());
#endif

  fs::writeStringToFileCompressed(fileName, j.dump());
}

Map *Map::loadMapFromFile(const std::string &fileName)
{
  std::string jsonAsString = fs::readCompressedFileAsString(fileName);

  if (jsonAsString.empty())
    return nullptr;

  json saveGameJSON = json::parse(jsonAsString, nullptr, false);

  if (saveGameJSON.is_discarded())
    throw ConfigurationError(TRACE_INFO "Could not parse savegame file " + fileName);

  size_t saveGameVersion = saveGameJSON.value("Savegame version", 0);

  if (saveGameVersion != SAVEGAME_VERSION)
  {
    /* @todo Check savegame version for compatibility and add upgrade functions here later if needed */
    throw CytopiaError(TRACE_INFO "Trying to load a Savegame with version " + std::to_string(saveGameVersion) +
                       " but only save-games with version " + std::to_string(SAVEGAME_VERSION) + " are supported");
  }

  int columns = saveGameJSON.value("columns", -1);
  int rows = saveGameJSON.value("rows", -1);

  if (columns == -1 || rows == -1)
    return nullptr;

  Map *map = new Map(columns, rows, false);
  map->mapNodes.reserve(columns * rows);

  for (const auto &it : saveGameJSON["mapNode"].items())
  {
    Point coordinates = json(it.value())["coordinates"].get<Point>();
    // set coordinates (height) of the map
    map->mapNodes.emplace_back(MapNode{Point{coordinates.x, coordinates.y, coordinates.z, coordinates.height}, ""});
    // load back mapNodeData (tileIDs, Buildins, ...)
    map->mapNodes.back().setMapNodeData(json(it.value())["mapNodeData"], coordinates);
  }

  // Now put those newly created nodes in correct drawing order
  for (int x = 0; x < columns; x++)
  {
    for (int y = columns - 1; y >= 0; y--)
    {
      map->mapNodesInDrawingOrder.emplace_back(&map->mapNodes[x * columns + y]);
    }
  }

  map->updateAllNodes();

  return map;
}

bool Map::isAllowSetTileId(const Layer layer, const MapNode *const pMapNode)
{
  switch (layer)
  {
  // Lisa: I disabled this check. This should not be explicitly forbidden by roads and rather be handled via the isOverplacable flag.
  // case Layer::ROAD:
  //   // During road construction, do not place new road tile over the old one
  //   if (pMapNode->isLayerOccupied(layer))
  //   {
  //     return false;
  //   }
  //   break;
  case Layer::ZONE:
    if ((pMapNode->isLayerOccupied(Layer::BUILDINGS) &&
         pMapNode->getMapNodeDataForLayer(Layer::BUILDINGS).tileData->category != "Flora") ||
        pMapNode->isLayerOccupied(Layer::WATER) || pMapNode->isLayerOccupied(Layer::ROAD) || pMapNode->isSlopeNode())
    {
      return false;
    }
    break;
  case Layer::WATER:
    if (pMapNode->isLayerOccupied(Layer::BUILDINGS) &&
        pMapNode->getMapNodeDataForLayer(Layer::BUILDINGS).tileData->category != "Flora")
    {
      return false;
    }
    break;
  default:
    break;
  }

  return true;
}

void Map::calculateVisibleMap(void)
{
  const Point topLeft = calculateIsoCoordinates({0, 0});
  const Point bottomRight = calculateIsoCoordinates({Settings::instance().screenWidth, Settings::instance().screenHeight});

  // Screen edges
  const int left = topLeft.x + topLeft.y - 2;
  const int right = bottomRight.x + bottomRight.y + 1;
  const int top = topLeft.y - topLeft.x + 1;
  // Lower the bottom because of high terrain nodes under the screen which will be pushed into the view
  const int bottom = bottomRight.y - bottomRight.x - 1 - MapNode::maxHeight;

  m_visibleNodesCount = 0;

  // ZOrder starts from topmost node to the right. (0,127) =1,(1,127) =2, ...
  for (int y = m_columns - 1; y >= 0; y--)
  {
    for (int x = 0; x < m_rows; x++)
    {
      const int xVal = x + y;
      const int yVal = y - x;

      if ((xVal >= left) && (xVal <= right) && (yVal <= top) && (yVal >= bottom))
      {
        pMapNodesVisible[m_visibleNodesCount++] = getMapNode({x, y}).getSprite();
      }
    }
  }
}

void Map::setTileID(const std::string &tileID, Point coordinate)
{
  TileData *tileData = TileManager::instance().getTileData(tileID);
  std::vector<Point> targetCoordinates = TileManager::instance().getTargetCoordsOfTileID(coordinate, tileID);
  bool allowed = false;

  if (!tileData || targetCoordinates.empty())
  { // if the node would not outside of map boundaries, targetCoordinates would be empty
    return;
  }

  if (!isPlacementOnAreaAllowed(targetCoordinates, tileID))
    return;

  Layer layer = TileManager::instance().getTileLayer(tileID);
  std::string randomGroundDecorationTileID;
  std::vector<Point> nodesToBeUpdated;

  // if this building has groundDeco, grab a random tileID from the list
  if (!tileData->groundDecoration.empty())
  {
    randomGroundDecorationTileID =
        *Randomizer::instance().choose(tileData->groundDecoration.begin(), tileData->groundDecoration.end());
  }

  // for >1x1 buildings, clear all the nodes that are going to be occupied before placing anything.
  if (targetCoordinates.size() >= 1 && layer == +Layer::BUILDINGS)
  {
    demolishNode(targetCoordinates, 0, Layer::FLORA);      // remove trees under the buildings
    demolishNode(targetCoordinates, 0, Layer::POWERLINES); // remove power lines under buildings
  }

  for (auto coord : targetCoordinates)
  { // now we can place our building

    MapNode &currentMapNode = getMapNode(coord);

    if (coord != coordinate && targetCoordinates.size() > 1)
    { // for buildings >1x1 set every node on the layer that will be occupied to invisible exepct of the origin node
      currentMapNode.getSprite()->setRenderFlag(layer, false);
    }
    else
    { // 1x1 buildings should be set to visible
      currentMapNode.getSprite()->setRenderFlag(layer, true);
    }

    if (!targetCoordinates.size() == 1)
    { // if it's not a >1x1 building, place tileID on the current coordinate (e.g. ground decoration beneath a > 1x1 building)
      currentMapNode.setTileID(tileID, coord);
    }
    else
    { // set the tileID for the mapNode of the origin coordinates only on the origin coordinate
      {
        currentMapNode.setTileID(tileID, coordinate);
      }
    }

    // place ground deco if we have one
    if (!randomGroundDecorationTileID.empty())
    {
      currentMapNode.setTileID(randomGroundDecorationTileID, coord);
    }

    // For layers that autotile to each other, we need to update their neighbors too
    if (TileManager::instance().isTileIDAutoTile(tileID))
    {
      nodesToBeUpdated.push_back(currentMapNode.getCoordinates());
    }
    // If we place a zone tile, add it to the ZoneManager
    // emit a signal to notify manager
    if (currentMapNode.getTileData(Layer::BUILDINGS) && currentMapNode.getTileData(Layer::ZONE))
    {
      SignalMediator::instance().signalPlaceBuilding.emit(currentMapNode);
    }
    else if (currentMapNode.getTileData(Layer::BUILDINGS) && currentMapNode.getTileData(Layer::BUILDINGS)->category == "Power")
    {
      SignalMediator::instance().signalPlacePowerBuilding.emit(currentMapNode);
    }
    else if (currentMapNode.getTileData(Layer::ZONE))
    {
      //signalPlaceZone.emit(currentMapNode);
      //SignalMediator::instance().test();
      SignalMediator::instance().signalPlaceZone.emit(currentMapNode);
    }
  }

  if (!nodesToBeUpdated.empty())
  {
    updateNodeNeighbors(nodesToBeUpdated);
  }
}

void Map::setTileID(const std::string &tileID, const std::vector<Point> &coordinates)
{
  for (auto coord : coordinates)
  {
    setTileID(tileID, coord);
  }
}
