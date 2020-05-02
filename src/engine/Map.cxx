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

void Map::getNodeInformation(const Point &isoCoordinates) const
{
  const TileData *tileData = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getActiveMapNodeData().tileData;
  LOG(LOG_INFO) << "===== TILE at " << isoCoordinates.x << ", " << isoCoordinates.y << "=====";
  LOG(LOG_INFO) << "[Layer: TERRAIN] ID: "
                << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getMapNodeDataForLayer(Layer::TERRAIN).tileID;
  LOG(LOG_INFO) << "[Layer: WATER] ID: "
                << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getMapNodeDataForLayer(Layer::WATER).tileID;
  LOG(LOG_INFO) << "[Layer: BUILDINGS] ID: "
                << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getMapNodeDataForLayer(Layer::BUILDINGS).tileID;
  LOG(LOG_INFO) << "Category: " << tileData->category;
  LOG(LOG_INFO) << "FileName: " << tileData->tiles.fileName;
  LOG(LOG_INFO) << "Rotations: " << tileData->tiles.rotations;
  LOG(LOG_INFO) << "TileMap: " << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getActiveMapNodeData().tileMap;
  LOG(LOG_INFO) << "TileIndex: " << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getActiveMapNodeData().tileIndex;
}

constexpr struct
{
  int x;
  int y;

} adjecantNodesOffsets[9]{
    {-1, -1}, // 6 = 2^6 = 64  = BOTTOM LEFT
    {-1, 0},  // 2 = 2^2 = 4   = LEFT
    {-1, 1},  // 4 = 2^4 = 16  = TOP LEFT
    {0, -1},  // 1 = 2^1 = 2   = BOTTOM
    {0, 0},   // center
    {0, 1},   // 0 = 2^0 = 1   = TOP
    {1, -1},  // 7 = 2^7 = 128 = BOTTOM RIGHT
    {1, 0},   // 3 = 2^3 = 8   = RIGHT
    {1, 1}    // 5 = 2^5 = 32  = TOP RIGHT
};

Map::Map(int columns, int rows) : m_columns(columns), m_rows(rows)
{
  const size_t vectorSize = static_cast<size_t>(m_rows * m_columns);
  randomEngine.seed();
  mapNodes.resize(vectorSize);
  mapNodesInDrawingOrder.reserve(vectorSize);
  MapLayers::enableLayer(Layer::TERRAIN);
  MapLayers::enableLayer(Layer::BUILDINGS);
  MapLayers::enableLayer(Layer::WATER);
  MapLayers::enableLayer(Layer::GROUND_DECORATION);
  MapLayers::enableLayer(Layer::ZONE);
  MapLayers::enableLayer(Layer::ROAD);
}

void Map::initMap()
{
  m_terrainGen.generateTerrain(mapNodes, mapNodesInDrawingOrder);
  updateAllNodes();
}

void Map::increaseHeight(const Point &isoCoordinates)
{
  const int height = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getCoordinates().height;

  if (height < Settings::instance().maxElevationHeight)
  {
    NeighborMatrix matrix;
    getNeighbors(isoCoordinates, matrix);
    for (const auto &it : matrix)
    {
      if (it && isPointWithinMapBoundaries(it->getCoordinates()) && it->isLayerOccupied(Layer::ZONE))
      {
        it->demolishLayer(Layer::ZONE);
      }
    }
    demolishNode(std::vector<Point>{isoCoordinates});
    mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->increaseHeight();
    updateNeighborsOfNode(mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getCoordinates());
    mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void Map::decreaseHeight(const Point &isoCoordinates)
{
  const int height = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getCoordinates().height;

  if (height > 0)
  {
    NeighborMatrix matrix;
    getNeighbors(isoCoordinates, matrix);
    for (const auto &it : matrix)
    {
      if (it && isPointWithinMapBoundaries(it->getCoordinates()) && it->isLayerOccupied(Layer::ZONE))
      {
        it->demolishLayer(Layer::ZONE);
      }
    }
    demolishNode(std::vector<Point>{isoCoordinates});
    mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->decreaseHeight();
    updateNeighborsOfNode(mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getCoordinates());
    mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void Map::updateNeighborsOfNode(const Point &isoCoordinates)
{
  const int tileHeight = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getCoordinates().height;

  NeighborMatrix matrix;
  getNeighbors(isoCoordinates, matrix);

  int i = 0;
  for (const auto &it : matrix)
  {
    if (it)
    {
      bool raise = false;
      const unsigned char elevationBitmask = getElevatedNeighborBitmask(it->getCoordinates());

      // if the elevation bitmask changes (-> a new texture is drawn), demolish the tile
      if (elevationBitmask != it->getElevationBitmask())
      {
        demolishNode(std::vector<Point>{it->getCoordinates()});
      }

      // set elevation and tile bitmask for each neighbor

      it->setBitmask(elevationBitmask, calculateAutotileBitmask(it->getCoordinates()));

      // there can't be a height difference greater then 1 between two map nodes.
      // only increase the cardinal directions
      if (i % 2)
      {
        if (tileHeight - it->getCoordinates().height > 1)
        {

          increaseHeight(it->getCoordinates());
        }
        else if (tileHeight - it->getCoordinates().height < -1)
        {
          decreaseHeight(it->getCoordinates());
        }
      }

      // those bitmask combinations require the tile to be elevated.
      std::vector<unsigned char> bits{3, 12, 26, 38, 73, 133};
      for (const auto &elevationBit : bits)
      {
        if ((elevationBitmask & elevationBit) == elevationBit)
        {
          raise = true;
        }
      }
      if (raise)
      {
        increaseHeight(it->getCoordinates());
        if (terrainEditMode == TerrainEdit::LOWER)
        {
          //decreaseHeight(it->getCoordinates());
          NeighborMatrix loweredNodesNeighbors;
          getNeighbors(it->getCoordinates(), loweredNodesNeighbors);
          for (const auto &neighbor : loweredNodesNeighbors)
          {
            if (neighbor && neighbor->getCoordinates().height > tileHeight)
            {
              decreaseHeight(neighbor->getCoordinates());
            }
          }
        }
      }
    }
    i++;
  }
}

void Map::updateAllNodes()
{
  for (const auto &it : mapNodes)
  {
    updateNeighborsOfNode(it->getCoordinates());
  }
}

bool Map::isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const
{
  if (TileManager::instance().getTileLayer(tileID) == Layer::ZONE)
  {
    return true;
  }
  if (mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y])
  {
    return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->isPlacementAllowed(tileID);
  }
  return false;
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
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;

  std::pair<int, int> adjecantNodesCoordinates[8]{
      std::make_pair(x, y + 1),     // 0 = 2^0 = 1   = TOP
      std::make_pair(x, y - 1),     // 1 = 2^1 = 2   = BOTTOM
      std::make_pair(x - 1, y),     // 2 = 2^2 = 4   = LEFT
      std::make_pair(x + 1, y),     // 3 = 2^3 = 8   = RIGHT
      std::make_pair(x - 1, y + 1), // 4 = 2^4 = 16  = TOP LEFT
      std::make_pair(x + 1, y + 1), // 5 = 2^5 = 32  = TOP RIGHT
      std::make_pair(x - 1, y - 1), // 6 = 2^6 = 64  = BOTTOM LEFT
      std::make_pair(x + 1, y - 1)  // 7 = 2^7 = 128 = BOTTOM RIGHT
  };

  unsigned int i = 0;
  for (const auto &it : adjecantNodesCoordinates)
  {
    if ((it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns) &&
        (mapNodes[it.first * m_columns + it.second]->getCoordinates().height >
             mapNodes[x * m_columns + y]->getCoordinates().height &&
         mapNodes[it.first * m_columns + it.second]))
    {
      // for each found tile add 2 ^ i to the bitmask
      bitmask |= (1u << i);
    }
    i++;
  }
  return bitmask;
}

Point Map::getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer)
{
  //Layer layer = TileManager::instance().getTileLayer(tileID);
  if (layer != Layer::NONE && isPointWithinMapBoundaries(isoCoordinates))
  {
    return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getOrigCornerPoint(layer);
  }
  return UNDEFINED_POINT;
}

std::vector<uint8_t> Map::calculateAutotileBitmask(const Point &isoCoordinates)
{
  std::vector<uint8_t> tileOrientationBitmask;
  tileOrientationBitmask.resize(LAYERS_COUNT);
  for (auto it : tileOrientationBitmask)
  {
    it = 0;
  }

  const int x = isoCoordinates.x;
  const int y = isoCoordinates.y;

  for (auto currentLayer : allLayersOrdered)
  {
    auto pCurrentTile = mapNodes[x * m_columns + y].get();

    if (pCurrentTile)
    {
      auto pCurrentTileData = pCurrentTile->getMapNodeDataForLayer(currentLayer).tileData;

      if (pCurrentTileData)
      {
        std::pair<int, int> adjecantNodesCoordinates[8]{
            std::make_pair(x, y + 1),     // 0 = 2^0 = 1   = TOP
            std::make_pair(x, y - 1),     // 1 = 2^1 = 2   = BOTTOM
            std::make_pair(x - 1, y),     // 2 = 2^2 = 4   = LEFT
            std::make_pair(x + 1, y),     // 3 = 2^3 = 8   = RIGHT
            std::make_pair(x - 1, y + 1), // 4 = 2^4 = 16  = TOP LEFT
            std::make_pair(x + 1, y + 1), // 5 = 2^5 = 32  = TOP RIGHT
            std::make_pair(x - 1, y - 1), // 6 = 2^6 = 64  = BOTTOM LEFT
            std::make_pair(x + 1, y - 1)  // 7 = 2^7 = 128 = BOTTOM RIGHT
        };

        if (pCurrentTileData->tileType == +TileType::TERRAIN)
        {
          unsigned int i = 0;
          for (const auto &it : adjecantNodesCoordinates)
          {
            if ((it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns) &&
                (mapNodes[it.first * m_columns + it.second] &&
                 mapNodes[it.first * m_columns + it.second]->getMapNodeDataForLayer(Layer::WATER).tileData &&
                 mapNodes[it.first * m_columns + it.second]->getMapNodeDataForLayer(Layer::WATER).tileData->tileType ==
                     +TileType::WATER))
            {
              // for each found tile add 2 ^ i to the bitmask
              tileOrientationBitmask[currentLayer] |= (1u << i);
            }
            i++;
          }
        }

        // only auto-tile categories that can be tiled.
        if (isLayerAutoTile(isoCoordinates, currentLayer))
        {
          unsigned int i = 0;
          for (const auto &it : adjecantNodesCoordinates)
          {
            const int secondTileIdx = it.first * m_columns + it.second;
            if ((it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns) &&
                (mapNodes[secondTileIdx] && mapNodes[secondTileIdx]->getMapNodeDataForLayer(currentLayer).tileData &&
                 ((mapNodes[secondTileIdx]->getMapNodeDataForLayer(currentLayer).tileID ==
                   pCurrentTile->getMapNodeDataForLayer(currentLayer).tileID) ||
                  (pCurrentTileData->tileType == +TileType::ROAD))))
            {
              // for each found tile add 2 ^ i to the bitmask
              tileOrientationBitmask[currentLayer] |= (1u << i);
            }
            i++;
          }
        }
      }
    }
  }
  return tileOrientationBitmask;
}

void Map::getNeighbors(const Point &isoCoordinates, NeighborMatrix &result) const
{
  size_t idx = 0;
  for (const auto &it : adjecantNodesOffsets)
  {
    const int x = isoCoordinates.x + it.x;
    const int y = isoCoordinates.y + it.y;
    if (x >= 0 && x < m_rows && y >= 0 && y < m_columns)
    {
      result[idx] = mapNodes[x * m_columns + y].get();
    }
    else
    {
      result[idx] = nullptr;
    }
    ++idx;
  }
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
      if (isClickWithinTile(screenCoordinates, x, y))
      {
        return mapNodes[x * m_columns + y]->getCoordinates();
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
    MapNode *node = mapNodes[isoCoord.x * m_columns + isoCoord.y].get();
    if (isPointWithinMapBoundaries(isoCoord))
    {
      // Check for multinode buildings first. Those are on the buildings layer, even if we want to demolish another layer than Buildings.
      // In case we add more Layers that support Multinode, add a for loop here
      // If demolishNode is called for layer GROUNDECORATION, we'll still need to gather all nodes from the multinode building to delete the decoration under the entire building
      if (node->getMapNodeDataForLayer(Layer::BUILDINGS).tileData && isNodeMultiObject(isoCoord))
      {
        const Point origCornerPoint = mapNodes[isoCoord.x * m_columns + isoCoord.y]->getOrigCornerPoint(Layer::BUILDINGS);
        const size_t origIndex = origCornerPoint.x * m_columns + origCornerPoint.y;

        if (origIndex < mapNodes.size() && mapNodes[origIndex])
        {
          const std::string &tileID = mapNodes[origIndex]->getTileID(Layer::BUILDINGS);
          std::vector<Point> objectCoordinates = getObjectCoords(origCornerPoint, tileID);

          for (auto coords : objectCoordinates)
          {
            if (std::find(nodesToDemolish.begin(), nodesToDemolish.end(),
                          mapNodes[coords.x * m_columns + coords.y]->getCoordinates()) == nodesToDemolish.end())
            {
              nodesToDemolish.push_back(mapNodes[coords.x * m_columns + coords.y]->getCoordinates());
            }
          }
        }
      }
      // make sure to add the points from the parameter to the vector if they're not in it (if they're a multiobject there'd be duplicates)
      if (std::find(nodesToDemolish.begin(), nodesToDemolish.end(),
                    mapNodes[isoCoord.x * m_columns + isoCoord.y]->getCoordinates()) == nodesToDemolish.end())
      {
        nodesToDemolish.push_back(mapNodes[isoCoord.x * m_columns + isoCoord.y]->getCoordinates());
      }
    }
  }

  for (auto &coords : nodesToDemolish)
  {
    mapNodes[coords.x * m_columns + coords.y]->demolishNode(layer);
    if (updateNeighboringTiles)
    {
      updateNeighborsOfNode({coords.x, coords.y});
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

    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(layer);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(layer);
    if (layer == Layer::TERRAIN)
      clipRect.h += 1; //HACK: We need to increase clipRect height by one pixel to match the drawRect. Rounding issue?

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      std::string tileID = mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(layer).tileID;
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      const int pixelX = static_cast<int>((screenCoordinates.x - spriteRect.x) / Camera::zoomLevel) + clipRect.x;
      const int pixelY = static_cast<int>((screenCoordinates.y - spriteRect.y) / Camera::zoomLevel) + clipRect.y;

      if (tileID.empty()) // should never happen, but better safe than sorry
      {
        break;
      }

      if (layer == Layer::TERRAIN &&
          mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::TERRAIN).tileMap == TileMap::SHORE)
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
    MapNode *node = mapNodes[index].get();
    node->getSprite()->highlightColor = rgbColor;
    node->getSprite()->highlightSprite = true;
  }
}

std::string Map::getTileID(const Point &isoCoordinates, Layer layer)
{
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;
  if (index < mapNodes.size())
  {
    MapNode *node = mapNodes[index].get();
    if (node)
    {
      return node->getTileID(layer);
    }
  }
  return "";
}

bool Map::isLayerAutoTile(const Point &isoCoordinates, const Layer &layer) const
{
  return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->isLayerAutoTile(layer);
}

void Map::unHighlightNode(const Point &isoCoordinates)
{
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;

  if (index < mapNodes.size())
  {
    MapNode *node = mapNodes[index].get();
    node->getSprite()->highlightSprite = false;
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
        std::make_unique<MapNode>(Point{coordinates.x, coordinates.y, coordinates.z, coordinates.height}, "");

    // load back mapNodeData (tileIDs, Buildins, ...)
    map->mapNodes[coordinates.x * columns + coordinates.y]->setMapNodeData(json(it.value())["mapNodeData"], coordinates);
  }

  // Now put those newly created nodes in correct drawing order
  for (int x = 0; x < columns; x++)
  {
    for (int y = columns - 1; y >= 0; y--)
    {
      map->mapNodesInDrawingOrder.push_back(map->mapNodes[x * columns + y].get());
    }
  }
  map->updateAllNodes();

  return map;
}

bool Map::isNodeMultiObject(const Point &isoCoordinates, Layer layer)
{
  if (isPointWithinMapBoundaries(isoCoordinates) && mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getTileData(layer))
  {
    MapNode *mapNode = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y].get();
    return (mapNode->getTileData(layer)->RequiredTiles.height > 1 && mapNode->getTileData(layer)->RequiredTiles.width > 1);
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
