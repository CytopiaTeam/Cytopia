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
  LOG(LOG_INFO) << "Biome: " << tileData->biome;
  LOG(LOG_INFO) << "Category: " << tileData->category;
  LOG(LOG_INFO) << "FileName: " << tileData->tiles.fileName;
  LOG(LOG_INFO) << "ID: " << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getActiveMapNodeData().tileID;
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
  mapNodes.resize(vectorSize);
  mapNodesInDrawingOrder.reserve(vectorSize);
  MapLayers::enableLayer(Layer::TERRAIN);
  MapLayers::enableLayer(Layer::BUILDINGS);
  MapLayers::enableLayer(Layer::WATER);
}

void Map::initMap()
{
  terrainGen.generateTerrain(mapNodes, mapNodesInDrawingOrder);
  updateAllNodes();
}

void Map::increaseHeight(const Point &isoCoordinates)
{
  const int height = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getCoordinates().height;

  if (height < Settings::instance().maxElevationHeight)
  {
    demolishNode(isoCoordinates);
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
    demolishNode(isoCoordinates);
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
        demolishNode(it->getCoordinates());
      }

      // set elevation and tile bitmask for each neighbor
      it->setBitmask(elevationBitmask, getNeighboringTilesBitmask(it->getCoordinates()));

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

  int i = 0;
  for (const auto &it : adjecantNodesCoordinates)
  {
    if ((it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns) &&
        (mapNodes[it.first * m_columns + it.second]->getCoordinates().height >
             mapNodes[x * m_columns + y]->getCoordinates().height &&
         mapNodes[it.first * m_columns + it.second]))
    {
      // for each found tile add 2 ^ i to the bitmask
      bitmask |= static_cast<unsigned int>(1 << i);
    }
    i++;
  }
  return bitmask;
}

Point Map::getNodeOrigCornerPoint(const Point &isoCoordinates, unsigned int layer)
{
  //Layer layer = TileManager::instance().getTileLayer(tileID);
  if (layer >= FIRST_LAYER)
  {
    return mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getOrigCornerPoint((Layer)layer);
  }
  return UNDEFINED_POINT;
}

unsigned char Map::getNeighboringTilesBitmask(const Point &isoCoordinates)
{
  unsigned char bitmask = 0;
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;

  // only auto-tile categories that can be tiled.
  if (mapNodes[x * m_columns + y] && mapNodes[x * m_columns + y]->getActiveMapNodeData().tileData &&
      (mapNodes[x * m_columns + y]->getActiveMapNodeData().tileData->tileType == +TileType::AUTOTILE ||
       mapNodes[x * m_columns + y]->getActiveMapNodeData().tileData->tileType == +TileType::UNDERGROUND))
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

    int i = 0;
    for (const auto &it : adjecantNodesCoordinates)
    {
      if ((it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns) &&
          (mapNodes[it.first * m_columns + it.second] &&
           mapNodes[it.first * m_columns + it.second]->getActiveMapNodeData().tileData && mapNodes[x * m_columns + y] &&
           mapNodes[x * m_columns + y]->getActiveMapNodeData().tileData &&
           mapNodes[it.first * m_columns + it.second]->getActiveMapNodeData().tileID ==
               mapNodes[x * m_columns + y]->getActiveMapNodeData().tileID))
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(1 << i);
      }
      i++;
    }
  }
  return bitmask;
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
  Point foundCoordinates{-1, -1, 0, 0};

  for (auto &it : mapNodes)
  {
    if (isClickWithinTile(screenCoordinates, it->getCoordinates().x, it->getCoordinates().y) &&
        (foundCoordinates.z <= it->getCoordinates().z))
    {
      foundCoordinates = it->getCoordinates();
    }
  }

  return foundCoordinates;
}

void Map::demolishNode(const Point &isoCoordinates, bool updateNeighboringTiles)
{
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;
  if (index < mapNodes.size())
  {
    Point origCornerPoint = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getOrigCornerPoint();

    Layer layer = Layer::BUILDINGS;
    const size_t origIndex = origCornerPoint.x * m_columns + origCornerPoint.y;

    if (origIndex < mapNodes.size() && mapNodes[origIndex])
    {
      std::string tileID = mapNodes[origIndex]->getTileID(layer);
      std::vector<Point> objectCoordinates = getObjectCoords(origCornerPoint, tileID);

      for (auto coords : objectCoordinates)
      {
        mapNodes[coords.x * m_columns + coords.y]->demolishNode();
      }
    }
    else
    {
      mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->demolishNode();
    }
    // TODO: Play soundeffect here
    if (updateNeighboringTiles)
    {
      updateNeighborsOfNode({isoCoordinates.x, isoCoordinates.y});
    }
  }
}

bool Map::isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY) const
{
  if (!isPointWithinMapBoundaries(isoX, isoY))
  {
    return false;
  }

  if (MapLayers::isLayerActive(Layer::BUILDINGS) && mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed(Layer::BUILDINGS))
  {
    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::BUILDINGS);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(Layer::BUILDINGS);

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      const int pixelX =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.x - spriteRect.x) / Camera::zoomLevel)) + clipRect.x;
      const int pixelY = static_cast<int>(std::round(static_cast<float>(screenCoordinates.y - spriteRect.y) / Camera::zoomLevel));

      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::BUILDINGS).tileID),
                                   pixelX, pixelY)
              .a != SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }
  // if we can't find the tile in the BUILDINGS layer, try terrain too. This means the cursor is on an occupied tile, but not on the building sprite itself
  if (MapLayers::isLayerActive(Layer::TERRAIN) && mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed(Layer::TERRAIN))
  {
    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::TERRAIN);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(Layer::TERRAIN);

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      // we need to offset the click coordinates by the clipping coordinates to check for the right sprite in the spritesheet.
      const int pixelX =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.x - spriteRect.x) / Camera::zoomLevel)) + clipRect.x;
      const int pixelY =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.y - spriteRect.y) / Camera::zoomLevel)) + clipRect.y;
      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::TERRAIN).tileID),
                                   pixelX, pixelY)
              .a != SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }

  // if we can't find the tile in the BUILDINGS layer, try terrain too. This means the cursor is on an occupied tile, but not on the building sprite itself
  if (MapLayers::isLayerActive(Layer::WATER) && mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed(Layer::WATER))
  {
    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::WATER);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(Layer::WATER);

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      // we need to offset the click coordinates by the clipping coordinates to check for the right sprite in the spritesheet.
      const int pixelX =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.x - spriteRect.x) / Camera::zoomLevel)) + clipRect.x;
      const int pixelY =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.y - spriteRect.y) / Camera::zoomLevel)) + clipRect.y;
      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::WATER).tileID),
                                   pixelX, pixelY)
              .a != SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }

  if (MapLayers::isLayerActive(Layer::BLUEPRINT) && mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed(Layer::BLUEPRINT))
  {
    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::BLUEPRINT);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(Layer::BLUEPRINT);

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      // we need to offset the click coordinates by the clipping coordinates to check for the right sprite in the spritesheet.
      const int pixelX =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.x - spriteRect.x) / Camera::zoomLevel)) + clipRect.x;
      const int pixelY =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.y - spriteRect.y) / Camera::zoomLevel)) + clipRect.y;
      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::BLUEPRINT).tileID),
                                   pixelX, pixelY)
              .a != SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }

  if (MapLayers::isLayerActive(Layer::UNDERGROUND) &&
      mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed(Layer::UNDERGROUND))
  {
    SDL_Rect spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::UNDERGROUND);
    SDL_Rect clipRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(Layer::UNDERGROUND);

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      // we need to offset the click coordinates by the clipping coordinates to check for the right sprite in the spritesheet.
      const int pixelX =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.x - spriteRect.x) / Camera::zoomLevel)) + clipRect.x;
      const int pixelY =
          static_cast<int>(std::round(static_cast<float>(screenCoordinates.y - spriteRect.y) / Camera::zoomLevel)) + clipRect.y;
      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::UNDERGROUND).tileID),
                                   pixelX, pixelY)
              .a != SDL_ALPHA_TRANSPARENT)
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
