#include "TileManager.hxx"

#include "LOG.hxx"
#include "Exception.hxx"
#include "basics/Settings.hxx"
#include "ResourcesManager.hxx"
#include "Filesystem.hxx"
#include "tileData.hxx"
#include "ThreadWorkers.hxx"
#include "../services/Randomizer.hxx"

#include <bitset>
#include <atomic>

using json = nlohmann::json;

TileManager::TileManager() { init(); }

TileData *TileManager::getTileData(const std::string &id) noexcept
{
  if (m_tileData.count(id))
    return &m_tileData[id];
  return nullptr;
}

std::vector<std::string> TileManager::getAllTileIDsForZone(ZoneType zone, ZoneDensity zoneDensity, TileSize tileSize)
{
  std::vector<std::string> results;
  for (auto &tileData : m_tileData)
  {
    if (std::find(tileData.second.zoneTypes.begin(), tileData.second.zoneTypes.end(), +zone) != tileData.second.zoneTypes.end() &&
        (zone == +ZoneType::AGRICULTURAL || std::find(tileData.second.zoneDensity.begin(), tileData.second.zoneDensity.end(),
        +zoneDensity) != tileData.second.zoneDensity.end()) &&
        tileData.second.RequiredTiles.height == tileSize.height && tileData.second.RequiredTiles.width == tileSize.width &&
        tileData.second.tileType != +TileType::ZONE)
    {
      results.push_back(tileData.first);
    }
  }
  return results;
}

std::vector<Point> TileManager::getTargetCoordsOfTileID(const Point &targetCoordinates, const std::string &tileID)
{
  std::vector<Point> occupiedCoords;
  TileData *tileData = getTileData(tileID);

  if (!tileData)
  {
    return occupiedCoords;
  }

  for (int i = 0; i < tileData->RequiredTiles.width; i++)
  {
    for (int j = 0; j < tileData->RequiredTiles.height; j++)
    {
      Point coords = {targetCoordinates.x - i, targetCoordinates.y + j};
      if (!coords.isWithinMapBoundaries())
      { // boundary check
        occupiedCoords.clear();
        return occupiedCoords;
      }
      occupiedCoords.emplace_back(coords);
    }
  }
  return occupiedCoords;
}

std::optional<std::string> TileManager::getRandomTileIDForZoneWithRandomSize(ZoneType zone, ZoneDensity zoneDensity,
                                                                             TileSize maxTileSize)

{
  std::vector<TileSize> elligibleTileSizes;

  // filter out the tilesizes that are below the maximum from all available tilesizes
  for (auto tileSize : m_tileSizeCombinations)
  {
    // for now only pick square buildings. non square buildings don't work yet.
    if (tileSize.height <= maxTileSize.height && tileSize.width <= maxTileSize.width && tileSize.height == tileSize.width)
    {
      elligibleTileSizes.push_back(tileSize);
    }
  }

  // pick a random tilesize from the new set of elligible tilesizes
  auto &randomizer = Randomizer::instance();
  TileSize randomTileSize = *randomizer.choose(elligibleTileSizes.begin(), elligibleTileSizes.end());

  // get all tile IDs for the according zone and tilesize
  const auto &tileIDsForThisZone = getAllTileIDsForZone(zone, zoneDensity, randomTileSize);

  if (tileIDsForThisZone.empty())
  {
    return std::nullopt;
  }

  // return a random tileID
  return *randomizer.choose(tileIDsForThisZone.begin(), tileIDsForThisZone.end());
}

Layer TileManager::getTileLayer(const std::string &tileID) const
{
  Layer layer = Layer::TERRAIN;
  TileData *tileData = TileManager::instance().getTileData(tileID);
  if (tileData)
  {
    switch (tileData->tileType)
    {
    case TileType::TERRAIN:
    layer = Layer::TERRAIN;
    break;
    case TileType::BLUEPRINT:
    layer = Layer::BLUEPRINT;
    break;
    case TileType::WATER:
    layer = Layer::WATER;
    break;
    case TileType::UNDERGROUND:
    layer = Layer::UNDERGROUND;
    break;
    case TileType::GROUNDDECORATION:
    layer = Layer::GROUND_DECORATION;
    break;
    case TileType::ZONE:
    layer = Layer::ZONE;
    break;
    case TileType::ROAD:
    layer = Layer::ROAD;
    break;
    case TileType::POWERLINE:
    layer = Layer::POWERLINES;
    break;
    case TileType::FLORA:
    layer = Layer::FLORA;
    break;
    default:
    layer = Layer::BUILDINGS;
    break;
    }
  }
  return layer;
}

size_t TileManager::calculateSlopeOrientation(unsigned char bitMaskElevation)
{
  // initialize with DEFAULT_ORIENTATION which elevationMask.none()
  size_t orientation = TileSlopes::DEFAULT_ORIENTATION;
  const std::bitset<8> elevationMask(bitMaskElevation);

  // Bits:
  // 0 = 2^0 = 1   = TOP
  // 1 = 2^1 = 2   = BOTTOM
  // 2 = 2^2 = 4   = LEFT
  // 3 = 2^3 = 8   = RIGHT
  // 4 = 2^4 = 16  = TOP LEFT
  // 5 = 2^5 = 32  = TOP RIGHT
  // 6 = 2^6 = 64  = BOTTOM LEFT
  // 7 = 2^7 = 128 = BOTTOM RIGHT

  // check for all combinations
  if (elevationMask.test(3) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::S_AND_E;
  }
  else if (elevationMask.test(2) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::N_AND_W;
  }
  else if (elevationMask.test(3) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::N_AND_E;
  }
  else if (elevationMask.test(2) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::S_AND_W;
  }

  else if (elevationMask.test(0) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::N_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::S_AND_E;
  }
  else if (elevationMask.test(0) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::N_AND_E;
  }
  else if (elevationMask.test(1) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::S_AND_W;
  }

  // diagonal combinations
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && RIGHT
    orientation = TileSlopes::N_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && LEFT
    orientation = TileSlopes::N_AND_E;
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && RIGHT
    orientation = TileSlopes::S_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && LEFT
    orientation = TileSlopes::S_AND_E;
  }

  // default directions
  else if (elevationMask.test(0))
  { // TOP
    orientation = TileSlopes::N;
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    orientation = TileSlopes::S;
  }
  else if (elevationMask.test(2))
  { // LEFT
    orientation = TileSlopes::W;
  }
  else if (elevationMask.test(3))
  { // RIGHT
    orientation = TileSlopes::E;
  }
  else if ((elevationMask.test(4) && elevationMask.test(7)) || (elevationMask.test(5) && elevationMask.test(6)))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::BETWEEN;
  }
  else if (elevationMask.test(4))
  { // TOP_LEFT
    orientation = TileSlopes::NW;
  }
  else if (elevationMask.test(5))
  { // TOP_RIGHT
    orientation = TileSlopes::NE;
  }
  else if (elevationMask.test(6))
  { // BOTTOM_LEFT
    orientation = TileSlopes::SW;
  }
  else if (elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = TileSlopes::SE;
  }

  return orientation;
}

TileOrientation TileManager::calculateTileOrientation(unsigned char bitMaskElevation)
{
  TileOrientation orientation;
  const std::bitset<8> elevationMask(bitMaskElevation);

  // Bits:
  // 0 = 2^0 = 1   = TOP
  // 1 = 2^1 = 2   = BOTTOM
  // 2 = 2^2 = 4   = LEFT
  // 3 = 2^3 = 8   = RIGHT
  // 4 = 2^4 = 16  = TOP LEFT
  // 5 = 2^5 = 32  = TOP RIGHT
  // 6 = 2^6 = 64  = BOTTOM LEFT
  // 7 = 2^7 = 128 = BOTTOM RIGHT

  // check for all combinations
  if (elevationMask.none())
  { // NONE
    orientation = TileOrientation::TILE_DEFAULT_ORIENTATION;
  }
  // special cases
  else if (elevationMask.test(0) && elevationMask.test(1) && elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_ALL_DIRECTIONS;
  }
  else if (elevationMask.test(0) && elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_N_AND_E_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_N_AND_E_AND_S;
  }
  else if (elevationMask.test(0) && elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_N_AND_S_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_S_AND_E_AND_W;
  }
  else if (elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_E_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(1))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::TILE_N_AND_S;
  }

  // diagonal combinations
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && RIGHT
    orientation = TileOrientation::TILE_N_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && LEFT
    orientation = TileOrientation::TILE_N_AND_E;
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && RIGHT
    orientation = TileOrientation::TILE_S_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && LEFT
    orientation = TileOrientation::TILE_S_AND_E;
  }

  // default directions
  else if (elevationMask.test(0))
  { // TOP
    orientation = TileOrientation::TILE_N;
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    orientation = TileOrientation::TILE_S;
  }
  else if (elevationMask.test(2))
  { // LEFT
    orientation = TileOrientation::TILE_W;
  }
  else if (elevationMask.test(3))
  { // RIGHT
    orientation = TileOrientation::TILE_E;
  }

  else
  {
    orientation = TILE_DEFAULT_ORIENTATION;
  }
  return orientation;
}

void TileManager::init()
{
  std::string jsonFile = fs::readFileAsString(Settings::instance().tileDataJSONFile.get());
  const json tileDataJSON = json::parse(jsonFile, nullptr, false);

  // check if json file can be parsed
  if (tileDataJSON.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().tileDataJSONFile.get());

  std::string key;

  size_t idx = 0;

  auto &threadWorkers = ThreadWorkers::instance();
  auto jsonItems = tileDataJSON.items();
  std::atomic_int tilesInProgress{0};
  uint32_t ticksStart = SDL_GetTicks();

  for (const auto &element : jsonItems)
  {
    std::string id;
    id = element.value().value("id", "");
    auto tileJson = std::make_shared<nlohmann::json>(tileDataJSON.at(idx));
    auto &tiled = m_tileData[id];
    tilesInProgress.fetch_add(1);

    threadWorkers.execute([this, tileJson = std::move(tileJson), id, &tiled, &tilesInProgress] {
      addJSONObjectToTileData(tiled, *tileJson, id);
      tilesInProgress.fetch_sub(1);
    });

    idx++;
  }

  // wait thread works
  while (tilesInProgress.load() != 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  LOG(LOG_DEBUG) << "Map Generated for " << ((float)(SDL_GetTicks() - ticksStart) / 1000.f) << " seconds";
}

void TileManager::addJSONObjectToTileData(TileData &tile, const nlohmann::json &tileJson, const std::string &id)
{
  tile.author = tileJson.value("author", "");
  tile.title = tileJson.value("title", "");
  tile.description = tileJson.value("description", "");
  tile.category = tileJson.value("category", "");
  tile.subCategory = tileJson.value("subCategory", "");
  tile.price = tileJson.value("price", 0);
  tile.power = tileJson.value("power", 0);
  tile.water = tileJson.value("water", 0);
  tile.upkeepCost = tileJson.value("upkeepCost", 0);
  tile.isOverPlacable = tileJson.value("isOverPlacable", false);
  tile.placeOnWater = tileJson.value("placeOnWater", false);
  tile.inhabitants = tileJson.value("inhabitants", 0);
  tile.happiness = tileJson.value("happiness", 0);
  tile.fireHazardLevel = tileJson.value("fireHazardLevel", 0);
  tile.educationLevel = tileJson.value("educationLevel", 0);
  tile.crimeLevel = tileJson.value("crimeLevel", 0);
  tile.pollutionLevel = tileJson.value("pollutionLevel", 0);

  std::string tileTypeStr = tileJson.value("tileType", "default");

  if (TileType::_is_valid_nocase(tileTypeStr.c_str()))
  {
    tile.tileType = TileType::_from_string_nocase(tileTypeStr.c_str());
  }
  else
  {
    throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                             " the field tileType uses the unsupported value " + tileTypeStr);
  }

  if (tileJson.find("zoneDensity") != tileJson.end())
  {
    for (const auto &zoneDensity : tileJson.at("zoneDensity").items())
    {
      if (ZoneDensity::_is_valid_nocase(zoneDensity.value().get<std::string>().c_str()))
      {
        tile.zoneDensity.push_back(ZoneDensity::_from_string_nocase(zoneDensity.value().get<std::string>().c_str()));
      }
      else
      {
        throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                                 " the field zoneDensity uses the unsupported value " + zoneDensity.value().get<std::string>());
      }
    }
  }

  if (tileJson.find("zoneType") != tileJson.end())
  {
    for (const auto &zoneType : tileJson.at("zoneType").items())
    {
      if (ZoneType::_is_valid_nocase(zoneType.value().get<std::string>().c_str()))
      {
        tile.zoneTypes.push_back(ZoneType::_from_string_nocase(zoneType.value().get<std::string>().c_str()));
      }
      else
      {
        throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                                 " the field zone uses the unsupported value " + zoneType.value().get<std::string>());
      }
    }
  }

  if (tileJson.find("style") != tileJson.end())
  {
    for (const auto &style : tileJson.at("style").items())
    {
      if (Style::_is_valid_nocase(style.value().get<std::string>().c_str()))
      {
        tile.style.push_back(Style::_from_string_nocase(style.value().get<std::string>().c_str()));
      }
      else
      {
        throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                                 " the field style uses the unsupported value " + style.value().get<std::string>());
      }
    }
  }

  if (tileJson.find("biomes") != tileJson.end())
  {
    for (const auto &biome : tileJson.at("biomes").items())
    {
      tile.biomes.push_back(biome.value().get<std::string>());
    }
  }

  if (tileJson.find("groundDecoration") != tileJson.end())
  {
    for (const auto &groundDecoration : tileJson.at("groundDecoration").items())
    {
      tile.groundDecoration.push_back(groundDecoration.value().get<std::string>());
    }
  }

  if (tileJson.find("RequiredTiles") != tileJson.end())
  {
    tile.RequiredTiles.width = tileJson["RequiredTiles"].value("width", 1);
    tile.RequiredTiles.height = tileJson["RequiredTiles"].value("height", 1);
  }
  else
  {
    tile.RequiredTiles.width = 1;
    tile.RequiredTiles.height = 1;
  }

  m_tileSizeCombinations.insert(tile.RequiredTiles);

  tile.tiles.fileName = tileJson["tiles"].value("fileName", "");
  tile.tiles.clippingHeight = tileJson["tiles"].value("clip_height", 0);
  tile.tiles.clippingWidth = tileJson["tiles"].value("clip_width", 0);

  // offset value can be negative in the json, for the tiledata editor, but never in Cytopia
  int offset = std::max(0, tileJson["tiles"].value("offset", 0));
  tile.tiles.offset = offset;
  tile.tiles.count = tileJson["tiles"].value("count", 1);
  tile.tiles.pickRandomTile = tileJson["tiles"].value("pickRandomTile", true);

  if (!m_tileData[id].tiles.fileName.empty())
  {
    ResourcesManager::instance().loadTileTextureAsync(id, tile.tiles.fileName);
  }

  if (tileJson.find("shoreLine") != tileJson.end())
  {
    tile.shoreTiles.fileName = tileJson["shoreLine"].value("fileName", "");
    tile.shoreTiles.count = tileJson["shoreLine"].value("count", 1);
    tile.shoreTiles.clippingWidth = tileJson["shoreLine"].value("clip_width", 0);
    tile.shoreTiles.clippingHeight = tileJson["shoreLine"].value("clip_height", 0);

    // offset value can be negative in the json, for the tiledata editor, but never in Cytopia
    offset = std::max(0, tileJson["shoreLine"].value("offset", 0));
    tile.shoreTiles.offset = offset;

    if (!tile.shoreTiles.fileName.empty())
    {
      ResourcesManager::instance().loadTileTextureAsync(id + "_shore", m_tileData[id].shoreTiles.fileName);
    }
  }

  if (tileJson.find("slopeTiles") != tileJson.end())
  {

    tile.slopeTiles.fileName = tileJson["slopeTiles"].value("fileName", "");
    tile.slopeTiles.count = tileJson["slopeTiles"].value("count", 1);
    tile.slopeTiles.clippingWidth = tileJson["slopeTiles"].value("clip_width", 0);
    tile.slopeTiles.clippingHeight = tileJson["slopeTiles"].value("clip_height", 0);

    // offset value can be negative in the json, for the tiledata editor, but never in Cytopia
    offset = std::max(0, tileJson["slopeTiles"].value("offset", 0));
    tile.slopeTiles.offset = offset;

    if (!tile.slopeTiles.fileName.empty())
    {
      ResourcesManager::instance().loadTileTextureAsync(id, tile.slopeTiles.fileName);
    }
  }
}

bool TileManager::isTileIDAutoTile(const std::string &tileID)
{
  TileData *tileData = getTileData(tileID);
  if (tileData)
    switch (tileData->tileType)
    {
    case +TileType::ROAD:
    case +TileType::AUTOTILE:
    case +TileType::UNDERGROUND:
    case +TileType::POWERLINE:
    return true;
    default:
    return false;
    }

  return false;
}
