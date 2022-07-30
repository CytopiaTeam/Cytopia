#include "TileManager.hxx"

#include "LOG.hxx"
#include "Exception.hxx"
#include "basics/Settings.hxx"
#include "ResourcesManager.hxx"
#include "Filesystem.hxx"
#include "tileData.hxx"
#include "../services/Randomizer.hxx"

#include <bitset>

using json = nlohmann::json;

TileManager::TileManager() { init(); }

SDL_Texture *TileManager::getTexture(const std::string &tileID) const
{
  return ResourcesManager::instance().getTileTexture(tileID);
}

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

  for (const auto &element : tileDataJSON.items())
  {
    std::string id;
    id = element.value().value("id", "");
    addJSONObjectToTileData(tileDataJSON, idx, id);
    idx++;
  }
}

void TileManager::addJSONObjectToTileData(const nlohmann::json &tileDataJSON, size_t idx, const std::string &id)
{
  m_tileData[id].author = tileDataJSON[idx].value("author", "");
  m_tileData[id].title = tileDataJSON[idx].value("title", "");
  m_tileData[id].description = tileDataJSON[idx].value("description", "");
  m_tileData[id].category = tileDataJSON[idx].value("category", "");
  m_tileData[id].subCategory = tileDataJSON[idx].value("subCategory", "");
  m_tileData[id].price = tileDataJSON[idx].value("price", 0);
  m_tileData[id].power = tileDataJSON[idx].value("power", 0);
  m_tileData[id].water = tileDataJSON[idx].value("water", 0);
  m_tileData[id].upkeepCost = tileDataJSON[idx].value("upkeepCost", 0);
  m_tileData[id].isOverPlacable = tileDataJSON[idx].value("isOverPlacable", false);
  m_tileData[id].placeOnWater = tileDataJSON[idx].value("placeOnWater", false);
  m_tileData[id].inhabitants = tileDataJSON[idx].value("inhabitants", 0);
  m_tileData[id].happiness = tileDataJSON[idx].value("happiness", 0);
  m_tileData[id].fireHazardLevel = tileDataJSON[idx].value("fireHazardLevel", 0);
  m_tileData[id].educationLevel = tileDataJSON[idx].value("educationLevel", 0);
  m_tileData[id].crimeLevel = tileDataJSON[idx].value("crimeLevel", 0);
  m_tileData[id].pollutionLevel = tileDataJSON[idx].value("pollutionLevel", 0);

  std::string tileTypeStr = tileDataJSON[idx].value("tileType", "default");

  if (TileType::_is_valid_nocase(tileTypeStr.c_str()))
  {
    m_tileData[id].tileType = TileType::_from_string_nocase(tileTypeStr.c_str());
  }
  else
  {
    throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                             " the field tileType uses the unsupported value " + tileTypeStr);
  }

  if (tileDataJSON[idx].find("zoneDensity") != tileDataJSON[idx].end())
  {
    for (auto zoneDensity : tileDataJSON[idx].at("zoneDensity").items())
    {
      if (ZoneDensity::_is_valid_nocase(zoneDensity.value().get<std::string>().c_str()))
      {
        m_tileData[id].zoneDensity.push_back(ZoneDensity::_from_string_nocase(zoneDensity.value().get<std::string>().c_str()));
      }
      else
      {
        throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                                 " the field zoneDensity uses the unsupported value " + zoneDensity.value().get<std::string>());
      }
    }
  }

  if (tileDataJSON[idx].find("zoneType") != tileDataJSON[idx].end())
  {
    for (auto zoneType : tileDataJSON[idx].at("zoneType").items())
    {
      if (ZoneType::_is_valid_nocase(zoneType.value().get<std::string>().c_str()))
      {
        m_tileData[id].zoneTypes.push_back(ZoneType::_from_string_nocase(zoneType.value().get<std::string>().c_str()));
      }
      else
      {
        throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                                 " the field zone uses the unsupported value " + zoneType.value().get<std::string>());
      }
    }
  }

  if (tileDataJSON[idx].find("style") != tileDataJSON[idx].end())
  {
    for (auto style : tileDataJSON[idx].at("style").items())
    {
      if (Style::_is_valid_nocase(style.value().get<std::string>().c_str()))
      {
        m_tileData[id].style.push_back(Style::_from_string_nocase(style.value().get<std::string>().c_str()));
      }
      else
      {
        throw ConfigurationError(TRACE_INFO "In TileData.json in field with ID " + id +
                                 " the field style uses the unsupported value " + style.value().get<std::string>());
      }
    }
  }

  if (tileDataJSON[idx].find("biomes") != tileDataJSON[idx].end())
  {
    for (auto biome : tileDataJSON[idx].at("biomes").items())
    {
      m_tileData[id].biomes.push_back(biome.value().get<std::string>());
    }
  }

  if (tileDataJSON[idx].find("groundDecoration") != tileDataJSON[idx].end())
  {
    for (auto groundDecoration : tileDataJSON[idx].at("groundDecoration").items())
    {
      m_tileData[id].groundDecoration.push_back(groundDecoration.value().get<std::string>());
    }
  }

  if (tileDataJSON[idx].find("RequiredTiles") != tileDataJSON[idx].end())
  {
    m_tileData[id].RequiredTiles.width = tileDataJSON[idx]["RequiredTiles"].value("width", 1);
    m_tileData[id].RequiredTiles.height = tileDataJSON[idx]["RequiredTiles"].value("height", 1);
  }
  else
  {
    m_tileData[id].RequiredTiles.width = 1;
    m_tileData[id].RequiredTiles.height = 1;
  }

  m_tileSizeCombinations.insert(m_tileData[id].RequiredTiles);

  m_tileData[id].tiles.fileName = tileDataJSON[idx]["tiles"].value("fileName", "");
  m_tileData[id].tiles.clippingHeight = tileDataJSON[idx]["tiles"].value("clip_height", 0);
  m_tileData[id].tiles.clippingWidth = tileDataJSON[idx]["tiles"].value("clip_width", 0);

  // offset value can be negative in the json, for the tiledata editor, but never in Cytopia
  int offset = tileDataJSON[idx]["tiles"].value("offset", 0);
  if (offset < 0)
  {
    offset = 0;
  }
  m_tileData[id].tiles.offset = offset;
  m_tileData[id].tiles.count = tileDataJSON[idx]["tiles"].value("count", 1);
  m_tileData[id].tiles.pickRandomTile = tileDataJSON[idx]["tiles"].value("pickRandomTile", true);

  if (!m_tileData[id].tiles.fileName.empty())
  {
    ResourcesManager::instance().loadTexture(id, m_tileData[id].tiles.fileName);
  }

  if (tileDataJSON[idx].find("shoreLine") != tileDataJSON[idx].end())
  {
    m_tileData[id].shoreTiles.fileName = tileDataJSON[idx]["shoreLine"].value("fileName", "");
    m_tileData[id].shoreTiles.count = tileDataJSON[idx]["shoreLine"].value("count", 1);
    m_tileData[id].shoreTiles.clippingWidth = tileDataJSON[idx]["shoreLine"].value("clip_width", 0);
    m_tileData[id].shoreTiles.clippingHeight = tileDataJSON[idx]["shoreLine"].value("clip_height", 0);

    // offset value can be negative in the json, for the tiledata editor, but never in Cytopia
    int offset = tileDataJSON[idx]["shoreLine"].value("offset", 0);
    if (offset < 0)
    {
      offset = 0;
    }
    m_tileData[id].shoreTiles.offset = offset;

    if (!m_tileData[id].shoreTiles.fileName.empty())
    {
      ResourcesManager::instance().loadTexture(id + "_shore", m_tileData[id].shoreTiles.fileName);
    }
  }

  if (tileDataJSON[idx].find("slopeTiles") != tileDataJSON[idx].end())
  {

    m_tileData[id].slopeTiles.fileName = tileDataJSON[idx]["slopeTiles"].value("fileName", "");
    m_tileData[id].slopeTiles.count = tileDataJSON[idx]["slopeTiles"].value("count", 1);
    m_tileData[id].slopeTiles.clippingWidth = tileDataJSON[idx]["slopeTiles"].value("clip_width", 0);
    m_tileData[id].slopeTiles.clippingHeight = tileDataJSON[idx]["slopeTiles"].value("clip_height", 0);

    // offset value can be negative in the json, for the tiledata editor, but never in Cytopia
    int offset = tileDataJSON[idx]["slopeTiles"].value("offset", 0);
    if (offset < 0)
    {
      offset = 0;
    }
    m_tileData[id].slopeTiles.offset = offset;

    if (!m_tileData[id].slopeTiles.fileName.empty())
    {
      ResourcesManager::instance().loadTexture(id, m_tileData[id].slopeTiles.fileName);
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
