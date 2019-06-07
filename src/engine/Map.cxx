#include "Map.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "basics/LOG.hxx"
#include "basics/compression.hxx"
#include "common/Constants.hxx"
#include "ResourcesManager.hxx"
#include "map/MapLayers.hxx"

#include "../ThirdParty/json.hxx"

#include <sstream>
#include <string>

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

using json = nlohmann::json;

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
}

void Map::initMap()
{
  MapLayers::enableLayer(Layer::TERRAIN);
  MapLayers::enableLayer(Layer::BUILDINGS);
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
            if (neighbor)
            {
              if (neighbor->getCoordinates().height > tileHeight)
              {
                decreaseHeight(neighbor->getCoordinates());
              }
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

void Map::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID)
{
  mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->setTileID(tileID);
  updateNeighborsOfNode(isoCoordinates);
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
    if (it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns)
    {
      if (mapNodes[it.first * m_columns + it.second]->getCoordinates().height >
              mapNodes[x * m_columns + y]->getCoordinates().height &&
          mapNodes[it.first * m_columns + it.second])
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(1 << i);
      }
    }
    i++;
  }
  return bitmask;
}

unsigned char Map::getNeighboringTilesBitmask(const Point &isoCoordinates)
{
  unsigned char bitmask = 0;
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;

  // only auto-tile categories that can be tiled.
  if (mapNodes[x * m_columns + y]->getActiveMapNodeData().tileData->category == "Roads")
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
      if (it.first >= 0 && it.first < m_rows && it.second >= 0 && it.second < m_columns)
      {
        if (mapNodes[it.first * m_columns + it.second]->getActiveMapNodeData().tileData->category ==
            mapNodes[x * m_columns + y]->getActiveMapNodeData().tileData->category)
        {
          // for each found tile add 2 ^ i to the bitmask
          bitmask |= static_cast<unsigned int>(1 << i);
        }
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

SDL_Color Map::getColorOfPixelInSurface(SDL_Surface *surface, int x, int y, const SDL_Rect &clipRect) const
{
  SDL_Color Color{0, 0, 0, SDL_ALPHA_TRANSPARENT};

  x += clipRect.w;

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

  // calculate clicked column (x coordinate) without height taken into account.
  const Point calculatedIsoCoords = calculateIsoCoordinates(screenCoordinates);
  int isoX = calculatedIsoCoords.x;
  int isoY = calculatedIsoCoords.y;

  // traverse a column from top to bottom (from the calculated coordinates)
  while (isoX < Settings::instance().mapSize && isoY < Settings::instance().mapSize && isoY >= 0)
  {
    if (isClickWithinTile(screenCoordinates, isoX, isoY))
    {
      if (foundCoordinates.z < mapNodes[isoX * m_columns + isoY]->getCoordinates().z)
      {
        foundCoordinates = mapNodes[isoX * m_columns + isoY]->getCoordinates();
      }
    }
    if (isoX > 0 && isClickWithinTile(screenCoordinates, isoX - 1, isoY))
    {
      if (foundCoordinates.z < mapNodes[(isoX - 1) * m_columns + isoY]->getCoordinates().z)
      {
        foundCoordinates = mapNodes[(isoX - 1) * m_columns + isoY]->getCoordinates();
      }
    }
    //check if isoY is already the last one
    if (isoY < Settings::instance().mapSize - 1 && isClickWithinTile(screenCoordinates, isoX, isoY + 1))
    {
      if (foundCoordinates.z < mapNodes[isoX * m_columns + (isoY + 1)]->getCoordinates().z)
      {
        foundCoordinates = mapNodes[isoX * m_columns + (isoY + 1)]->getCoordinates();
      }
    }

    isoX++;
    isoY--;
  }

  return foundCoordinates;
}

void Map::demolishNode(const Point &isoCoordinates, bool updateNeighboringTiles)
{
  mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->demolishNode();
  // TODO: Play soundeffect here
  if (updateNeighboringTiles)
  {
    updateNeighborsOfNode({isoCoordinates.x, isoCoordinates.y});
  }
}

bool Map::isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY) const
{
  if (isoX < 0 || isoX > Settings::instance().mapSize || isoY < 0 || isoY > Settings::instance().mapSize)
  {
    return false;
  }

  SDL_Rect spriteRect;
  //TODO: Add other layers later
  if (MapLayers::isLayerActive(Layer::BUILDINGS) &&
      mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed((Layer::BUILDINGS)))
  {
    spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::BUILDINGS);

    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      const int pixelX = static_cast<int>((screenCoordinates.x - spriteRect.x) / Camera::zoomLevel);
      const int pixelY = static_cast<int>((screenCoordinates.y - spriteRect.y) / Camera::zoomLevel);

      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::BUILDINGS).tileID,
                                       mapNodes[isoX * m_columns + isoY]->tileMap),
                                   pixelX, pixelY, mapNodes[isoX * m_columns + isoY]->getSprite()->getActiveClipRect())
              .a != SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }
  // if we can't find the tile in the BUILDINGS layer, try terrain too.
  if (MapLayers::isLayerActive(Layer::TERRAIN) && mapNodes[isoX * m_columns + isoY]->getSprite()->isLayerUsed((Layer::TERRAIN)))
  {
    spriteRect = mapNodes[isoX * m_columns + isoY]->getSprite()->getDestRect(Layer::TERRAIN);
    if (SDL_PointInRect(&screenCoordinates, &spriteRect))
    {
      // Calculate the position of the clicked pixel within the surface and "un-zoom" the position to match the un-adjusted surface
      const int pixelX = static_cast<int>((screenCoordinates.x - spriteRect.x) / Camera::zoomLevel);
      const int pixelY = static_cast<int>((screenCoordinates.y - spriteRect.y) / Camera::zoomLevel);

      // TODO: The problem here is, that the pixel check doesn't work with all terrain tiles. This is a pre-layer issue. idk what causes it yet.
      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(ResourcesManager::instance().getTileSurface(
                                       mapNodes[isoX * m_columns + isoY]->getMapNodeDataForLayer(Layer::TERRAIN).tileID,
                                       mapNodes[isoX * m_columns + isoY]->tileMap),
                                   pixelX, pixelY, mapNodes[isoX * m_columns + isoY]->getSprite()->getClipRect(Layer::TERRAIN))
              .a != SDL_ALPHA_TRANSPARENT)
      {
        return true;
      }
    }
  }
  // Nothing found
  return false;
}

void Map::highlightNode(const Point &isoCoordinates)
{
  const size_t index = isoCoordinates.x * m_columns + isoCoordinates.y;

  if (index < mapNodes.size())
  {
    MapNode *node = mapNodes[index].get();
    node->getSprite()->highlightSprite = true;
  }
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

  const std::string compressedSaveGame = compressString(j.dump());

  if (!compressedSaveGame.empty())
  {
    file << compressedSaveGame;
  }
  file.close();
}

Map *Map::loadMapFromFile(const std::string &fileName)
{
  std::ifstream file(SDL_GetBasePath() + fileName, std::ios_base::in | std::ios_base::binary);

  if (file.fail())
  {
    LOG(LOG_ERROR) << "Could not load savegame file " << fileName;
    return nullptr;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  std::string jsonAsString;
  jsonAsString = decompressString(buffer.str());

  if (jsonAsString.empty())
  {
    return nullptr;
  }

  json saveGameJSON = json::parse(jsonAsString, nullptr, false);

  file.close();

  if (saveGameJSON.is_discarded())
  {
    LOG(LOG_ERROR) << "Could not parse savegame file " << fileName;
    return nullptr;
  }

  size_t saveGameVersion = saveGameJSON.value("Savegame version", 0);

  if (saveGameVersion != SAVEGAME_VERSION)
  {
    // Check savegame version for compatibility and add upgrade functions here later if needed

    LOG(LOG_ERROR) << "Trying to load a Savegame with version " << saveGameVersion << " but only save-games with version."
                   << SAVEGAME_VERSION << " are supported";
  }

  int columns = saveGameJSON.value("columns", -1);
  int rows = saveGameJSON.value("rows", -1);

  if (columns == -1 || rows == -1)
    return nullptr;

  Map *map = new Map(columns, rows);
  map->initMap();

  // set coordinates first
  for (const auto &it : saveGameJSON["mapNode"].items())
  {
    Point coordinates = json(it.value())["coordinates"].get<Point>();

    // for now we only set tileID read from the savegame
    map->mapNodes[coordinates.x * columns + coordinates.y]->setCoordinates(coordinates);
  }

  // update all nodes to reflect new height differences and  to have correct tiling for new tileIDs
  map->updateAllNodes();

  // now set the tileIDs after the nodes have been updated, so they're not auto-demolished
  for (const auto &it : saveGameJSON["mapNode"].items())
  {
    Point coordinates = json(it.value())["coordinates"].get<Point>();

    // get mapNodeData and store in a vector
    std::vector<MapNodeData> tileIDs = json(it.value())["mapNodeData"];
    for (const auto &data : tileIDs)
    {
      if (!data.tileID.empty())
      {
        map->mapNodes[coordinates.x * columns + coordinates.y]->setTileID(data.tileID);
      }
    }
  }
  //update again to auto tile roads and so on
  map->updateAllNodes();

  return map;
}

// JSON serializer for Point class
void to_json(json &j, const Point &point) { j = json{{"x", point.x}, {"y", point.y}, {"z", point.z}, {"height", point.height}}; }

// JSON serializer for MapNodeData struct
void to_json(json &j, const MapNodeData &mapNodeData) { j = json{{"tileID", mapNodeData.tileID}}; }

// JSON serializer for MapNode class
void to_json(json &j, const std::unique_ptr<MapNode> &m)
{
  if (m.get())
  {
    j = json{{"coordinates", m->getCoordinates()}, {"mapNodeData", m->getMapNodeData()}};
  }
  else
  {
    j = nullptr;
  }
}

// JSON deserializer for Point class
void from_json(const json &j, Point &point)
{
  point.x = j.at("x").get<int>();
  point.y = j.at("y").get<int>();
  point.z = j.at("z").get<int>();
  point.height = j.at("height").get<int>();
}

// JSON deserializer for Point class
void from_json(const json &j, MapNodeData &mapNodeData) { mapNodeData.tileID = j.at("tileID").get<std::string>(); }

void Map::getNodeInformation(const Point &isoCoordinates) const
{
  const TileData *tileData = mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getActiveMapNodeData().tileData;
  LOG() << "===== TILE at " << isoCoordinates.x << ", " << isoCoordinates.y << "=====";
  LOG() << "Biome: " << tileData->biome;
  LOG() << "Category: " << tileData->category;
  LOG() << "FileName: " << tileData->tiles.fileName;
  LOG() << "ID: " << mapNodes[isoCoordinates.x * m_columns + isoCoordinates.y]->getActiveMapNodeData().tileID;
}
