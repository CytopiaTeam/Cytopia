#include "Map.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "LOG.hxx"
#include "basics/compression.hxx"
#include "common/Constants.hxx"
#include "ResourcesManager.hxx"
#include "map/MapLayers.hxx"
#include "common/JsonSerialization.hxx"
#include "Filesystem.hxx"
#include "../view/Window.hxx"
#include "json.hxx"

#include <sstream>
#include <string>
#include <set>
#include <queue>
#include <unordered_set>

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

using json = nlohmann::json;

Window * Map::m_Window = nullptr;

NeighbourNodesPosition operator++(NeighbourNodesPosition &nn, int)
{
  NeighbourNodesPosition res = nn;

  switch (nn)
  {
  case NeighbourNodesPosition::BOTOM_LEFT:
    nn = NeighbourNodesPosition::LEFT;
    break;
  case NeighbourNodesPosition::LEFT:
    nn = NeighbourNodesPosition::TOP_LEFT;
    break;
  case NeighbourNodesPosition::TOP_LEFT:
    nn = NeighbourNodesPosition::BOTTOM;
    break;
  case NeighbourNodesPosition::BOTTOM:
    nn = NeighbourNodesPosition::CENTAR;
    break;
  case NeighbourNodesPosition::CENTAR:
    nn = NeighbourNodesPosition::TOP;
    break;
  case NeighbourNodesPosition::TOP:
    nn = NeighbourNodesPosition::BOTOM_RIGHT;
    break;
  case NeighbourNodesPosition::BOTOM_RIGHT:
    nn = NeighbourNodesPosition::RIGHT;
    break;
  case NeighbourNodesPosition::RIGHT:
    nn = NeighbourNodesPosition::TOP_RIGHT;
    break;
  case NeighbourNodesPosition::TOP_RIGHT:
    nn = NeighbourNodesPosition::BOTOM_LEFT;
    break;
  default:
    assert(false);
    break;
  }

  return res;
}

void Map::getNodeInformation(const Point &isoCoordinates) const
{
  const MapNode &mapNode = mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)];
  const MapNodeData &mapNodeData = mapNode.getActiveMapNodeData();
  const TileData *tileData = mapNodeData.tileData;
  LOG(LOG_INFO) << "===== TILE at " << isoCoordinates.x << ", " << isoCoordinates.y << ", " << mapNode.getCoordinates().height
                << "=====";
  LOG(LOG_INFO) << "[Layer: TERRAIN] ID: " << mapNode.getMapNodeDataForLayer(Layer::TERRAIN).tileID;
  LOG(LOG_INFO) << "[Layer: WATER] ID: " << mapNode.getMapNodeDataForLayer(Layer::WATER).tileID;
  LOG(LOG_INFO) << "[Layer: BUILDINGS] ID: " << mapNode.getMapNodeDataForLayer(Layer::BUILDINGS).tileID;
  LOG(LOG_INFO) << "Category: " << tileData->category;
  LOG(LOG_INFO) << "FileName: " << tileData->tiles.fileName;
  LOG(LOG_INFO) << "Rotations: " << tileData->tiles.rotations;
  LOG(LOG_INFO) << "TileMap: " << mapNodeData.tileMap;
  LOG(LOG_INFO) << "TileIndex: " << mapNodeData.tileIndex;
}

Map::Map(int columns, int rows, const bool generateTerrain)
    : pMapNodesVisible(new Sprite *[columns * rows]), m_columns(columns), m_rows(rows)
{
  // TODO move Random Engine out of map
  randomEngine.seed();
  MapLayers::enableLayers({TERRAIN, BUILDINGS, WATER, GROUND_DECORATION, ZONE, ROAD});

  if (generateTerrain)
  {
    m_terrainGen.generateTerrain(mapNodes, mapNodesInDrawingOrder);
  }

  updateAllNodes();
}

Map::~Map() { delete[] pMapNodesVisible; }

std::vector<NeighborNode> Map::getNeighborNodes(const Point &isoCoordinates, const bool includeCentralNode)
{
  constexpr int neighborRange = 1;
  std::vector<NeighborNode> neighbors;
  NeighbourNodesPosition position = NeighbourNodesPosition::BOTOM_LEFT;

  for (int xOffset = -neighborRange; xOffset <= neighborRange; ++xOffset)
  {
    for (int yOffset = -neighborRange; yOffset <= neighborRange; ++yOffset, position++)
    {
      if (!includeCentralNode && (xOffset == 0) && (yOffset == 0))
      {
        continue;
      }

      const int neighborX = isoCoordinates.x + xOffset;
      const int neighborY = isoCoordinates.y + yOffset;

      if (isPointWithinMapBoundaries(neighborX, neighborY))
      {
        neighbors.push_back({&mapNodes[nodeIdx(neighborX, neighborY)], position});
      }
    }
  }

  return neighbors;
}

bool Map::updateHeight(MapNode &mapNode, const bool higher, std::vector<NeighborNode> &neighbors)
{
  if (mapNode.changeHeight(higher))
  {
    for (const auto neighbour : neighbors)
    {
      if (neighbour.pNode->isLayerOccupied(Layer::ZONE))
      {
        neighbour.pNode->demolishLayer(Layer::ZONE);
      }
    }

    return true;
  }

  return false;
}

void Map::changeHeight(const Point &isoCoordinates, const bool higher)
{
  MapNode &mapNode = mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)];
  std::vector<MapNode *> nodesToUpdate{&mapNode};
  auto neighbours = getNeighborNodes(isoCoordinates, true);

  if (updateHeight(mapNode, higher, neighbours))
  {
    demolishNode({isoCoordinates});

    // If lowering node height, than all nodes around should be lowered to be on same height with the central one.
    if (!higher)
    {
      const int centerHeight = mapNode.getCoordinates().height;

      for (auto &neighbour : neighbours)
      {
        if (centerHeight < neighbour.pNode->getCoordinates().height)
        {
          neighbour.pNode->changeHeight(false);
          demolishNode({neighbour.pNode->getCoordinates()});
          nodesToUpdate.push_back(neighbour.pNode);
        }
      }
    }

    updateNodeNeighbors(nodesToUpdate);
  }
}

void Map::increaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, true); }

void Map::decreaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, false); }

void Map::updateNodeNeighbors(std::vector<MapNode *> &nodes)
{
  // those bitmask combinations require the tile to be elevated.
  constexpr unsigned char elevateTileComb[] = {
      NeighbourNodesPosition::TOP | NeighbourNodesPosition::BOTTOM,
      NeighbourNodesPosition::LEFT | NeighbourNodesPosition::RIGHT,
      NeighbourNodesPosition::TOP_LEFT | NeighbourNodesPosition::RIGHT | NeighbourNodesPosition::BOTTOM,
      NeighbourNodesPosition::TOP_RIGHT | NeighbourNodesPosition::LEFT | NeighbourNodesPosition::BOTTOM,
      NeighbourNodesPosition::BOTOM_LEFT | NeighbourNodesPosition::RIGHT | NeighbourNodesPosition::TOP,
      NeighbourNodesPosition::BOTOM_RIGHT | NeighbourNodesPosition::LEFT | NeighbourNodesPosition::TOP};

  std::unordered_set<MapNode *> nodesToBeUpdated;
  std::map<MapNode *, std::vector<NeighborNode>> nodeCache;
  std::queue<MapNode *> nodesUpdatedHeight;
  std::vector<MapNode *> nodesToElevate;
  std::unordered_set<MapNode *> nodesToDemolish;

  for (auto &pUpdateNode : nodes)
  {
    nodesUpdatedHeight.push(pUpdateNode);

    while (!nodesUpdatedHeight.empty() || !nodesToElevate.empty())
    {
      while (!nodesUpdatedHeight.empty())
      {
        const auto pHeighChangedNode = nodesUpdatedHeight.front();
        nodesUpdatedHeight.pop();
        const int tileHeight = pHeighChangedNode->getCoordinates().height;

        if (nodeCache.count(pHeighChangedNode) == 0)
        {
          nodeCache[pHeighChangedNode] = getNeighborNodes(pHeighChangedNode->getCoordinates(), false);
        }

        if (std::find(nodesToElevate.begin(), nodesToElevate.end(), pHeighChangedNode) == nodesToElevate.end())
        {
          nodesToElevate.push_back(pHeighChangedNode);
        }

        for (const auto &neighbour : nodeCache[pHeighChangedNode])
        {
          const auto pNode = neighbour.pNode;
          const auto &nodeCoordinate = pNode->getCoordinates();
          const int heightDiff = tileHeight - nodeCoordinate.height;

          if (nodeCache.count(pNode) == 0)
          {
            nodeCache[pNode] = getNeighborNodes(pNode->getCoordinates(), false);
          }

          if (std::find(nodesToElevate.begin(), nodesToElevate.end(), pNode) == nodesToElevate.end())
          {
            nodesToElevate.push_back(pNode);
          }

          if (std::abs(heightDiff) > 1)
          {
            nodesUpdatedHeight.push(pNode);
            updateHeight(*pNode, (heightDiff > 1) ? true : false, nodeCache[pNode]);
          }
        }
      }

      while (nodesUpdatedHeight.empty() && !nodesToElevate.empty())
      {
        MapNode *pEleNode = nodesToElevate.back();
        nodesToBeUpdated.insert(pEleNode);
        nodesToElevate.pop_back();

        if (nodeCache.count(pEleNode) == 0)
        {
          nodeCache[pEleNode] = getNeighborNodes(pEleNode->getCoordinates(), false);
        }

        const unsigned char elevationBitmask = getElevatedNeighborBitmask(pEleNode, nodeCache[pEleNode]);

        if (elevationBitmask != pEleNode->getElevationBitmask())
        {
          nodesToDemolish.insert(pEleNode);
          pEleNode->setElevationBitMask(elevationBitmask);
        }

        for (const auto &elBitMask : elevateTileComb)
        {
          if ((elevationBitmask & elBitMask) == elBitMask)
          {
            updateHeight(*pEleNode, true, nodeCache[pEleNode]);
            nodesUpdatedHeight.push(pEleNode);
            break;
          }
        }
      }
    }
  }

  if (!nodesToDemolish.empty())
  {
    std::vector<Point> nodesToDemolishV(nodesToDemolish.size());
    std::transform(nodesToDemolish.begin(), nodesToDemolish.end(), nodesToDemolishV.begin(),
                   [](MapNode *mn) { return mn->getCoordinates(); });
    demolishNode(nodesToDemolishV);
  }

  for (auto pNode : nodesToBeUpdated)
  {
    pNode->setAutotileBitMask(calculateAutotileBitmask(pNode, nodeCache[pNode]));
  }

  for (auto pNode : nodesToBeUpdated)
  {
    pNode->updateTexture();
  }
}

void Map::updateAllNodes() { updateNodeNeighbors(mapNodesInDrawingOrder); }

bool Map::isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const
{
  if (TileManager::instance().getTileLayer(tileID) == Layer::ZONE)
  {
    return true;
  }

  return mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)].isPlacementAllowed(tileID);
}

std::vector<Point> Map::getObjectCoords(const Point &isoCoordinates, const std::string &tileID)
{
  std::vector<Point> ret;
  TileData *tileData = TileManager::instance().getTileData(tileID);

  if (!tileData)
  {
    return ret;
  }

  Point coords = isoCoordinates;

  for (int i = 0; i < tileData->RequiredTiles.width; i++)
  {
    for (int j = 0; j < tileData->RequiredTiles.height; j++)
    {
      coords.x = isoCoordinates.x - i;
      coords.y = isoCoordinates.y + j;
      ret.emplace_back(coords);
    }
  }

  return ret;
}

unsigned char Map::getElevatedNeighborBitmask(MapNode *pMapNode, const std::vector<NeighborNode> &neighbors)
{
  unsigned char bitmask = 0;
  const auto centralNodeHeight = pMapNode->getCoordinates().height;

  for (const auto &neighbour : neighbors)
  {
    if (neighbour.pNode->getCoordinates().height > centralNodeHeight)
    {
      bitmask |= neighbour.position;
    }
  }

  return bitmask;
}

Point Map::getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer)
{
  if ((layer != Layer::NONE) && isPointWithinMapBoundaries(isoCoordinates))
  {
    return mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)].getOrigCornerPoint(layer);
  }

  return Point::INVALID();
}

std::vector<uint8_t> Map::calculateAutotileBitmask(const MapNode *const pMapNode, const std::vector<NeighborNode> &neighborNodes)
{
  std::vector<uint8_t> tileOrientationBitmask(LAYERS_COUNT, 0);

  for (auto currentLayer : allLayersOrdered)
  {
    auto pCurrentTileData = pMapNode->getMapNodeDataForLayer(currentLayer).tileData;

    if (pCurrentTileData)
    {
      if (pCurrentTileData->tileType == +TileType::TERRAIN)
      {
        for (const auto &neighbour : neighborNodes)
        {
          const auto pTileData = neighbour.pNode->getMapNodeDataForLayer(Layer::WATER).tileData;

          if (pTileData && pTileData->tileType == +TileType::WATER)
          {
            tileOrientationBitmask[currentLayer] |= neighbour.position;
          }
        }
      }

      // only auto-tile categories that can be tiled.
      if (pMapNode->isLayerAutoTile(currentLayer))
      {
        const auto nodeTileId = pMapNode->getMapNodeDataForLayer(currentLayer).tileID;

        for (const auto &neighbour : neighborNodes)
        {
          const MapNodeData &nodeData = neighbour.pNode->getMapNodeDataForLayer(currentLayer);

          if (nodeData.tileData && ((nodeData.tileID == nodeTileId) || (pCurrentTileData->tileType == +TileType::ROAD)))
          {
            tileOrientationBitmask[currentLayer] |= neighbour.position;
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

Point Map::findNodeInMap(const SDL_Point &screenCoordinates, const Layer &layer) const
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

#ifndef NDEBUG
  int zOrder = INT_MAX;
#endif
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
#ifndef NDEBUG
      // Assert assumption that we test all nodes in correct Z order
      assert(zOrder > mapNodes[nodeIdx(x, y)].getCoordinates().z);
      zOrder = mapNodes[nodeIdx(x, y)].getCoordinates().z;
#endif
      if (isClickWithinTile(screenCoordinates, x, y, layer))
      {
        return mapNodes[nodeIdx(x, y)].getCoordinates();
      }
    }
  }

  return Point{-1, -1, 0, 0};
}

void Map::demolishNode(const std::vector<Point> &isoCoordinates, bool updateNeighboringTiles, Layer layer)
{
  std::unordered_set<MapNode *> nodesToDemolish;

  for (auto &isoCoord : isoCoordinates)
  {
    if (isPointWithinMapBoundaries(isoCoord))
    {
      MapNode &node = mapNodes[nodeIdx(isoCoord.x, isoCoord.y)];

      // Check for multi-node buildings first. Those are on the buildings layer, even if we want to demolish another layer than Buildings.
      // In case we add more Layers that support Multi-node, add a for loop here
      // If demolishNode is called for layer GROUNDECORATION, we'll still need to gather all nodes from the multi-node building to delete the decoration under the entire building
      auto pNodeTileData = node.getMapNodeDataForLayer(Layer::BUILDINGS).tileData;

      if (pNodeTileData && ((pNodeTileData->RequiredTiles.height > 1) || (pNodeTileData->RequiredTiles.width > 1)))
      {
        const Point origCornerPoint = node.getOrigCornerPoint(Layer::BUILDINGS);
        const size_t origIndex = nodeIdx(origCornerPoint.x, origCornerPoint.y);

        if (origIndex < mapNodes.size())
        {
          const std::string &tileID = mapNodes[origIndex].getTileID(Layer::BUILDINGS);
          std::vector<Point> objectCoordinates = getObjectCoords(origCornerPoint, tileID);

          for (auto coords : objectCoordinates)
          {
            nodesToDemolish.insert(&mapNodes[nodeIdx(coords.x, coords.y)]);
          }
        }
      }

      nodesToDemolish.insert(&node);
    }
  }

  std::vector<MapNode *> updateNodes;
  for (auto pNode : nodesToDemolish)
  {
    pNode->demolishNode(layer);
    // TODO: Play sound effect here
    if (updateNeighboringTiles)
    {
      updateNodes.push_back(pNode);
    }
  }

  if (!updateNodes.empty())
  {
    updateNodeNeighbors(updateNodes);
  }
}

bool Map::isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY, const Layer &layer = Layer::NONE) const
{
  if (!isPointWithinMapBoundaries(isoX, isoY))
  {
    return false;
  }

  auto &node = mapNodes[nodeIdx(isoX, isoY)];
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
  const size_t index = nodeIdx(isoCoordinates.x, isoCoordinates.y);

  if (index < mapNodes.size())
  {
    const auto pSprite = mapNodes[index].getSprite();
    pSprite->highlightColor = rgbColor;
    pSprite->highlightSprite = true;
  }
}

std::string Map::getTileID(const Point &isoCoordinates, Layer layer)
{
  const size_t index = nodeIdx(isoCoordinates.x, isoCoordinates.y);
  return (index < mapNodes.size()) ? mapNodes[index].getTileID(layer) : "";
}

void Map::unHighlightNode(const Point &isoCoordinates)
{
  const size_t index = nodeIdx(isoCoordinates.x, isoCoordinates.y);

  if (index < mapNodes.size())
  {
    mapNodes[index].getSprite()->highlightSprite = false;
  }
}

void Map::saveMapToFile(const std::string &fileName)
{
  //create savegame json string
  const json j =
      json{{"Savegame version", SAVEGAME_VERSION}, {"columns", this->m_columns}, {"rows", this->m_rows}, {"mapNode", mapNodes}};

#ifdef DEBUG
  // Write uncompressed savegame for easier debugging
  fs::writeStringToFile(fileName + ".txt", j.dump());
#endif

  const std::string compressedSaveGame = compressString(j.dump());

  if (!compressedSaveGame.empty())
  {
    fs::writeStringToFile(fileName, compressedSaveGame, true);
  }
}

Map *Map::loadMapFromFile(const std::string &fileName)
{
  std::string jsonAsString = decompressString(fs::readFileAsString(fileName, true));

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
  case Layer::ROAD:
    // During road construction, do not place new road tile over the old one
    if (pMapNode->isLayerOccupied(layer))
    {
      return false;
    }
    break;
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
  if(!m_Window)
  {
    throw CytopiaError{TRACE_INFO "Cannot calculateVisibleMap without a Window"};
  }
  const Point topLeft = calculateIsoCoordinates({0, 0});
  const Point bottomRight = calculateIsoCoordinates({m_Window->getBounds().width(), m_Window->getBounds().height()});

  // Screen edges
  const int left = topLeft.x + topLeft.y - 2;
  const int right = bottomRight.x + bottomRight.y + 1;
  const int top = topLeft.y - topLeft.x + 1;
  // Lower the bottom because of high terrain nodes under the screen which will be pushed into the view
  const int bottom = bottomRight.y - bottomRight.x - 1 - MapNode::maxHeight;

  m_visibleNodesCount = 0;

  for (int x = 0; x < m_rows; x++)
  {
    for (int y = m_columns - 1; y >= 0; y--)
    {
      const int xVal = x + y;
      const int yVal = y - x;

      if ((xVal >= left) && (xVal <= right) && (yVal <= top) && (yVal >= bottom))
      {
        pMapNodesVisible[m_visibleNodesCount++] = mapNodes[nodeIdx(x, y)].getSprite();
      }
    }
  }
}

void Map::setWindow(Window * window) {
  m_Window = window;
}
