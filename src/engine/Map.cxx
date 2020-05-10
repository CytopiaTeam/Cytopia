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

#include "json.hxx"

#include <sstream>
#include <string>

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

using json = nlohmann::json;

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
  LOG(LOG_INFO) << "===== TILE at " << isoCoordinates.x << ", " << isoCoordinates.y << "=====";
  LOG(LOG_INFO) << "[Layer: TERRAIN] ID: " << mapNode.getMapNodeDataForLayer(Layer::TERRAIN).tileID;
  LOG(LOG_INFO) << "[Layer: WATER] ID: " << mapNode.getMapNodeDataForLayer(Layer::WATER).tileID;
  LOG(LOG_INFO) << "[Layer: BUILDINGS] ID: " << mapNode.getMapNodeDataForLayer(Layer::BUILDINGS).tileID;
  LOG(LOG_INFO) << "Category: " << tileData->category;
  LOG(LOG_INFO) << "FileName: " << tileData->tiles.fileName;
  LOG(LOG_INFO) << "Rotations: " << tileData->tiles.rotations;
  LOG(LOG_INFO) << "TileMap: " << mapNodeData.tileMap;
  LOG(LOG_INFO) << "TileIndex: " << mapNodeData.tileIndex;
}

Map::Map(int columns, int rows) : m_columns(columns), m_rows(rows)
{
  const size_t vectorSize = static_cast<size_t>(m_rows * m_columns);
  // TODO move Random Engine out of map
  randomEngine.seed();
  MapLayers::enableLayer({TERRAIN, BUILDINGS, WATER, GROUND_DECORATION, ZONE, ROAD});
  m_terrainGen.generateTerrain(mapNodes, mapNodesInDrawingOrder);
  updateAllNodes();
}

std::vector<NeighbourNode> Map::getNeighborNodes(const Point &isoCoordinates, const bool includeCentralNode)
{
  constexpr int neighborRange = 1;
  std::vector<NeighbourNode> neighbors;
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

void Map::changeHeight(const Point &isoCoordinates, const bool higher)
{
  constexpr int minElevationHeight = 0;
  MapNode &mapNode = mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)];
  const Point nodeCoords = mapNode.getCoordinates();

  if (mapNode.changeHeight(higher))
  {
    for (const auto neighbour : getNeighborNodes(isoCoordinates, true))
    {
      if (neighbour.pNode->isLayerOccupied(Layer::ZONE))
      {
        neighbour.pNode->demolishLayer(Layer::ZONE);
      }
    }

    demolishNode({isoCoordinates});
    updateNodeNeighbors(nodeCoords);
    mapNode.getSprite()->refresh();
  }
}

void Map::increaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, true); }

void Map::decreaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, false); }

void Map::updateNodeNeighbors(const Point &isoCoordinates)
{
  const int tileHeight = mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)].getCoordinates().height;
  constexpr unsigned char cardinalDirection =
      NeighbourNodesPosition::TOP | NeighbourNodesPosition::BOTTOM | NeighbourNodesPosition::LEFT | NeighbourNodesPosition::RIGHT;
  // those bitmask combinations require the tile to be elevated.
  constexpr unsigned char elevateTileComb[] = {
      NeighbourNodesPosition::TOP | NeighbourNodesPosition::BOTTOM,
      NeighbourNodesPosition::LEFT | NeighbourNodesPosition::RIGHT,
      NeighbourNodesPosition::TOP_LEFT | NeighbourNodesPosition::RIGHT | NeighbourNodesPosition::BOTTOM,
      NeighbourNodesPosition::TOP_RIGHT | NeighbourNodesPosition::LEFT | NeighbourNodesPosition::BOTTOM,
      NeighbourNodesPosition::BOTOM_LEFT | NeighbourNodesPosition::RIGHT | NeighbourNodesPosition::TOP,
      NeighbourNodesPosition::BOTOM_RIGHT | NeighbourNodesPosition::LEFT | NeighbourNodesPosition::TOP};

  for (const auto &neighbour : getNeighborNodes(isoCoordinates, true))
  {
    const auto pNode = neighbour.pNode;
    bool raise = false;
    const auto &nodeCoordinate = pNode->getCoordinates();
    const unsigned char elevationBitmask = getElevatedNeighborBitmask(nodeCoordinate);

    // if the elevation bitmask changes (-> a new texture is drawn), demolish the tile
    if (elevationBitmask != pNode->getElevationBitmask())
    {
      demolishNode({nodeCoordinate});
    }

    // set elevation and tile bitmask for each neighbor
    pNode->setBitmask(elevationBitmask, calculateAutotileBitmask(nodeCoordinate));

    // there can't be a height difference greater then 1 between two map nodes. Only increase the cardinal directions.
    if (cardinalDirection & neighbour.position)
    {
      const int heightDiff = tileHeight - nodeCoordinate.height;

      if (std::abs(heightDiff) > 1)
      {
        changeHeight(nodeCoordinate, (heightDiff > 1) ? true : false);
      }
    }

    for (const auto &elBitMask : elevateTileComb)
    {
      if ((elevationBitmask & elBitMask) == elBitMask)
      {
        increaseHeight(nodeCoordinate);

        if (terrainEditMode == TerrainEdit::LOWER)
        {
          for (const auto &nn : getNeighborNodes(pNode->getCoordinates(), true))
          {
            const auto &neighborCoords = nn.pNode->getCoordinates();

            if (neighborCoords.height > tileHeight)
            {
              decreaseHeight(neighborCoords);
            }
          }
        }

        break;
      }
    }
  }
}

void Map::updateAllNodes()
{
  for (const auto &it : mapNodes)
  {
    updateNodeNeighbors(it.getCoordinates());
  }
}

bool Map::isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const
{
  if (TileManager::instance().getTileLayer(tileID) == Layer::ZONE)
  {
    return true;
  }

  return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y].isPlacementAllowed(tileID);
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
      ret.push_back(coords);
    }
  }
  return ret;
}

unsigned char Map::getElevatedNeighborBitmask(const Point &isoCoordinates)
{
  unsigned char bitmask = 0;
  const auto centralNodeHeight = mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)].getCoordinates().height;

  for (const auto &neighbour : getNeighborNodes(isoCoordinates, false))
  {
    if ((neighbour.pNode->getCoordinates().height > centralNodeHeight))
    {
      bitmask |= neighbour.position;
    }
  }

  return bitmask;
}

Point Map::getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer)
{
  //Layer layer = TileManager::instance().getTileLayer(tileID);
  if (layer != Layer::NONE && isPointWithinMapBoundaries(isoCoordinates))
  {
    return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y].getOrigCornerPoint(layer);
  }
  return UNDEFINED_POINT;
}

std::vector<uint8_t> Map::calculateAutotileBitmask(const Point &isoCoordinates)
{
  std::vector<uint8_t> tileOrientationBitmask(LAYERS_COUNT, 0);

  for (auto currentLayer : allLayersOrdered)
  {
    MapNode &mapNode = mapNodes[nodeIdx(isoCoordinates.x, isoCoordinates.y)];
    auto pCurrentTileData = mapNode.getMapNodeDataForLayer(currentLayer).tileData;

    if (pCurrentTileData)
    {
      const auto &neighbourNodes = getNeighborNodes(isoCoordinates, false);

      if (pCurrentTileData->tileType == +TileType::TERRAIN)
      {
        for (const auto &neighbour : neighbourNodes)
        {
          const auto pTileData = neighbour.pNode->getMapNodeDataForLayer(Layer::WATER).tileData;

          if ((pTileData && pTileData->tileType == +TileType::WATER))
          {
            tileOrientationBitmask[currentLayer] |= neighbour.position;
          }
        }
      }

      // only auto-tile categories that can be tiled.
      if (isLayerAutoTile(isoCoordinates, currentLayer))
      {
        for (const auto &neighbour : neighbourNodes)
        {
          const MapNodeData &nodeData = neighbour.pNode->getMapNodeDataForLayer(currentLayer);

          if (nodeData.tileData && ((nodeData.tileID == mapNode.getMapNodeDataForLayer(currentLayer).tileID) ||
                                    (pCurrentTileData->tileType == +TileType::ROAD)))
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

  for (const auto &it : mapNodesInDrawingOrder)
  {
    it->render();
  }
}

void Map::refresh()
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Refresh Map", MP_YELLOW);
#endif
  for (const auto &it : mapNodesInDrawingOrder)
  {
    it->getSprite()->refresh();
  }
}

SDL_Color Map::getColorOfPixelInSurface(SDL_Surface *surface, int x, int y) const
{
  SDL_Color Color{0, 0, 0, SDL_ALPHA_TRANSPARENT};
  if (surface)
  {
    const int bpp = surface->format->BytesPerPixel;
    Uint8 *p = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp;
    const Uint32 pixel = *reinterpret_cast<Uint32 *>(p);

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }

  return Color;
}

Point Map::findNodeInMap(const SDL_Point &screenCoordinates) const
{
  // calculate clicked column (x coordinate) without height taken into account.
  const Point calculatedIsoCoords = calculateIsoCoordinates(screenCoordinates);
  int isoX = calculatedIsoCoords.x;
  int isoY = calculatedIsoCoords.y;

  // adjust caluclated values that are outside of the map (which is legit, but they need to get pushed down)
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
  // Transerse a column form from calculated coordinates to the bottom of the map.
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
      // Assert asumption that we test all nodes in correct Z order
      assert(zOrder > mapNodes[x * m_columns + y].getCoordinates().z);
      zOrder = mapNodes[x * m_columns + y].getCoordinates().z;
#endif
      if (isClickWithinTile(screenCoordinates, x, y))
      {
        return mapNodes[x * m_columns + y].getCoordinates();
      }
    }
  }

  return Point{-1, -1, 0, 0};
}

void Map::demolishNode(const std::vector<Point> &isoCoordinates, bool updateNeighboringTiles, Layer layer)
{
  std::vector<Point> nodesToDemolish;
  for (auto &isoCoord : isoCoordinates)
  {
    MapNode &node = mapNodes[isoCoord.x * m_columns + isoCoord.y];
    if (isPointWithinMapBoundaries(isoCoord))
    {
      // Check for multinode buildings first. Those are on the buildings layer, even if we want to demolish another layer than Buildings.
      // In case we add more Layers that support Multinode, add a for loop here
      // If demolishNode is called for layer GROUNDECORATION, we'll still need to gather all nodes from the multinode building to delete the decoration under the entire building
      if (node.getMapNodeDataForLayer(Layer::BUILDINGS).tileData && isNodeMultiObject(isoCoord))
      {
        const Point origCornerPoint = node.getOrigCornerPoint(Layer::BUILDINGS);
        const size_t origIndex = origCornerPoint.x * m_columns + origCornerPoint.y;

        if (origIndex < mapNodes.size())
        {
          const std::string &tileID = mapNodes[origIndex].getTileID(Layer::BUILDINGS);
          std::vector<Point> objectCoordinates = getObjectCoords(origCornerPoint, tileID);

          for (auto coords : objectCoordinates)
          {
            if (std::find(nodesToDemolish.begin(), nodesToDemolish.end(),
                          mapNodes[coords.x * m_columns + coords.y].getCoordinates()) == nodesToDemolish.end())
            {
              nodesToDemolish.push_back(mapNodes[coords.x * m_columns + coords.y].getCoordinates());
            }
          }
        }
      }
      // make sure to add the points from the parameter to the vector if they're not in it (if they're a multiobject there'd be duplicates)
      if (std::find(nodesToDemolish.begin(), nodesToDemolish.end(),
                    mapNodes[isoCoord.x * m_columns + isoCoord.y].getCoordinates()) == nodesToDemolish.end())
      {
        nodesToDemolish.push_back(mapNodes[isoCoord.x * m_columns + isoCoord.y].getCoordinates());
      }
    }
  }

  for (auto &coords : nodesToDemolish)
  {
    mapNodes[coords.x * m_columns + coords.y].demolishNode(layer);
    if (updateNeighboringTiles)
    {
      updateNodeNeighbors({coords.x, coords.y});
    }
    // TODO: Play soundeffect here
  }
}

bool Map::isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY) const
{
  if (!isPointWithinMapBoundaries(isoX, isoY))
  {
    return false;
  }

  // Layers ordered for hitcheck
  Layer layers[] = {Layer::BUILDINGS, Layer::TERRAIN, Layer::WATER, Layer::UNDERGROUND, Layer::BLUEPRINT};

  for (auto &layer : layers)
  {
    if (!MapLayers::isLayerActive(layer))
    {
      continue;
    }

    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY].getSprite()->getDestRect(layer);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY].getSprite()->getClipRect(layer);
    if (layer == Layer::TERRAIN)
      clipRect.h += 1; //HACK: We need to increase clipRect height by one pixel to match the drawRect. Rounding issue?

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      std::string tileID = mapNodes[isoX * m_columns + isoY].getMapNodeDataForLayer(layer).tileID;
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      const int pixelX = static_cast<int>((screenCoordinates.x - spriteRect.x) / Camera::zoomLevel) + clipRect.x;
      const int pixelY = static_cast<int>((screenCoordinates.y - spriteRect.y) / Camera::zoomLevel) + clipRect.y;

      if (tileID.empty()) // should never happen, but better safe than sorry
      {
        break;
      }

      if (layer == Layer::TERRAIN &&
          mapNodes[isoX * m_columns + isoY].getMapNodeDataForLayer(Layer::TERRAIN).tileMap == TileMap::SHORE)
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
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;

  if (index < mapNodes.size())
  {
    const MapNode &node = mapNodes[index];
    node.getSprite()->highlightColor = rgbColor;
    node.getSprite()->highlightSprite = true;
  }
}

std::string Map::getTileID(const Point &isoCoordinates, Layer layer)
{
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;
  if (index < mapNodes.size())
  {
    return mapNodes[index].getTileID(layer);
  }

  return "";
}

bool Map::isLayerAutoTile(const Point &isoCoordinates, const Layer &layer) const
{
  return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y].isLayerAutoTile(layer);
}

void Map::unHighlightNode(const Point &isoCoordinates)
{
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;

  if (index < mapNodes.size())
  {
    mapNodes[index].getSprite()->highlightSprite = false;
  }
}

void Map::saveMapToFile(const std::string &fileName)
{
  const json j =
      json{{"Savegame version", SAVEGAME_VERSION}, {"columns", this->m_columns}, {"rows", this->m_rows}, {"mapNode", mapNodes}};

  std::ofstream file(SDL_GetBasePath() + fileName, std::ios_base::out | std::ios_base::binary);

#ifdef DEBUG
  // Write uncompressed savegame for easier debugging
  std::ofstream fileuncompressed(SDL_GetBasePath() + fileName + "_UNCOMPRESSED", std::ios_base::out | std::ios_base::binary);
  fileuncompressed << j.dump();
#endif

  const std::string compressedSaveGame = compressString(j.dump());

  if (!compressedSaveGame.empty())
  {
    file << compressedSaveGame;
  }
  file.close();
}

Map *Map::loadMapFromFile(const std::string &fileName)
{
  std::stringstream buffer;
  {
    std::ifstream file(SDL_GetBasePath() + fileName, std::ios_base::in | std::ios_base::binary);
    if (!file)
      throw ConfigurationError(TRACE_INFO "Could not load savegame file " + fileName);
    buffer << file.rdbuf();
  }

  std::string jsonAsString;
  jsonAsString = decompressString(buffer.str());

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

  Map *map = new Map(columns, rows);

  for (const auto &it : saveGameJSON["mapNode"].items())
  {
    Point coordinates = json(it.value())["coordinates"].get<Point>();
    // set coordinates (height) of the map
    map->mapNodes[coordinates.x * columns + coordinates.y] =
        std::move(MapNode{Point{coordinates.x, coordinates.y, coordinates.z, coordinates.height}, ""});

    // load back mapNodeData (tileIDs, Buildins, ...)
    map->mapNodes[coordinates.x * columns + coordinates.y].setMapNodeData(json(it.value())["mapNodeData"], coordinates);
  }

  // Now put those newly created nodes in correct drawing order
  for (int x = 0; x < columns; x++)
  {
    for (int y = columns - 1; y >= 0; y--)
    {
      map->mapNodesInDrawingOrder.push_back(&map->mapNodes[x * columns + y]);
    }
  }
  map->updateAllNodes();

  return map;
}

bool Map::isNodeMultiObject(const Point &isoCoordinates, Layer layer)
{
  if (isPointWithinMapBoundaries(isoCoordinates) && mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y].getTileData(layer))
  {
    const MapNode &mapNode = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y];
    return ((mapNode.getTileData(layer)->RequiredTiles.height > 1) && (mapNode.getTileData(layer)->RequiredTiles.width > 1));
  }
  return false;
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
  default:
    break;
  }

  return true;
}
