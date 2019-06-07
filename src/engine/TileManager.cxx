#include "TileManager.hxx"

#include "basics/LOG.hxx"
#include "basics/Settings.hxx"
#include "ResourcesManager.hxx"

#include <bitset>

using json = nlohmann::json;

TileManager::TileManager() { init(); }

SDL_Texture *TileManager::getTexture(const std::string &id, size_t tileMapType) const
{
  return ResourcesManager::instance().getTileTexture(id, tileMapType);
}

TileData *TileManager::getTileData(const std::string &id)
{
  if (m_tileData.count(id))
  {
    return &m_tileData[id];
  }
  else
  {
    LOG(LOG_ERROR) << "Non existing id is beeing accessed at Tile::getTileData(id) " << id;
  }

  return nullptr;
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

size_t TileManager::calculateTileOrientation(unsigned char bitMaskElevation)
{
  size_t orientation;
  std::bitset<8> elevationMask(bitMaskElevation);

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
    orientation = TileList::TILE_DEFAULT_ORIENTATION;
  }
  // special cases
  else if (elevationMask.test(0) && elevationMask.test(1) && elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_ALL_DIRECTIONS;
  }
  else if (elevationMask.test(0) && elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_N_AND_E_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_N_AND_E_AND_S;
  }
  else if (elevationMask.test(0) && elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_N_AND_S_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_S_AND_E_AND_W;
  }
  else if (elevationMask.test(2) && elevationMask.test(3))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_E_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(1))
  { // BOTTOM_RIGHT
    orientation = TileList::TILE_N_AND_S;
  }

  // diagonal combinations
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && RIGHT
    orientation = TileList::TILE_N_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && LEFT
    orientation = TileList::TILE_N_AND_E;
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && RIGHT
    orientation = TileList::TILE_S_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && LEFT
    orientation = TileList::TILE_S_AND_E;
  }

  // default directions
  else if (elevationMask.test(0))
  { // TOP
    orientation = TileList::TILE_N;
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    orientation = TileList::TILE_S;
  }
  else if (elevationMask.test(2))
  { // LEFT
    orientation = TileList::TILE_W;
  }
  else if (elevationMask.test(3))
  { // RIGHT
    orientation = TileList::TILE_E;
  }

  else
  {
    orientation = TILE_DEFAULT_ORIENTATION;
  }
  return orientation;
}

void TileManager::init()
{
  // Read JSON File.
  std::ifstream i(SDL_GetBasePath() + Settings::instance().tileDataJSONFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << Settings::instance().tileDataJSONFile << " does not exist!";
    return;
  }

  // check if json file can be parsed
  const json tileDataJSON = json::parse(i, nullptr, false);
  if (tileDataJSON.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << Settings::instance().tileDataJSONFile;
  }
  i.close();

  std::string key;

  size_t idx = 0;

  for (auto element : tileDataJSON.items())
  {
    // check if ID is an array and multiple sprites are supplied. If that's the case, we create separate m_tileData entries for each element.
    if (element.value()["id"].is_array())
    {
      size_t count = element.value()["tiles"].value("count", -1);
      if (element.value()["id"].size() != count)
      {
        std::string title = element.value().value("title", "");
        LOG(LOG_ERROR) << "There are " << count << " elements in the section \"tiles\" of element with Title: " << title
                       << " but only " << element.value()["id"].size() << " IDs!";
      }
      for (const auto &it : element.value()["id"].items())
      {
        addJSONObjectToTileData(tileDataJSON, idx, it.value(), std::stoi(it.key()));
      }
    }
    else
    {
      std::string id;
      id = element.value().value("id", "");
      addJSONObjectToTileData(tileDataJSON, idx, id);
    }

    idx++;
  }
}

void TileManager::addJSONObjectToTileData(const nlohmann::json &tileDataJSON, size_t idx, const std::string &id, int tileIndex)
{
  m_tileData[id].tiles.clippingWidth = tileDataJSON[idx]["tiles"].value("clip_width", 0);
  m_tileData[id].tiles.count = tileDataJSON[idx]["tiles"].value("count", 1);
  m_tileData[id].author = tileDataJSON[idx].value("author", "");
  m_tileData[id].title = tileDataJSON[idx].value("title", "");
  m_tileData[id].description = tileDataJSON[idx].value("description", "");
  m_tileData[id].category = tileDataJSON[idx].value("category", "");
  m_tileData[id].price = tileDataJSON[idx].value("price", 0);
  m_tileData[id].water = tileDataJSON[idx].value("water", 0);
  m_tileData[id].drawGround = tileDataJSON[idx].value("draw ground", false);

  m_tileData[id].tiles.fileName = tileDataJSON[idx]["tiles"].value("fileName", "");
  m_tileData[id].tiles.clippingHeight = tileDataJSON[idx]["tiles"].value("clip_height", 0);

  if (!m_tileData[id].tiles.fileName.empty())
  {
    ResourcesManager::instance().loadTexture(id, m_tileData[id].tiles.fileName, TileMap::DEFAULT);
  }

  if (tileIndex != -1)
  {
    m_tileData[id].tileIndex = tileIndex;
  }

  if (tileDataJSON[idx].find("cornerTiles") != tileDataJSON[idx].end())
  {
    m_tileData[id].cornerTiles.fileName = tileDataJSON[idx]["cornerTiles"].value("fileName", "");
    m_tileData[id].cornerTiles.count = tileDataJSON[idx]["cornerTiles"].value("count", 1);
    m_tileData[id].cornerTiles.clippingWidth = tileDataJSON[idx]["cornerTiles"].value("clip_width", 0);
    m_tileData[id].cornerTiles.clippingHeight = tileDataJSON[idx]["cornerTiles"].value("clip_height", 0);

    if (!m_tileData[id].cornerTiles.fileName.empty())
    {
      ResourcesManager::instance().loadTexture(id, m_tileData[id].cornerTiles.fileName, TileMap::CORNERS);
    }
  }

  if (tileDataJSON[idx].find("slopeTiles") != tileDataJSON[idx].end())
  {

    m_tileData[id].slopeTiles.fileName = tileDataJSON[idx]["slopeTiles"].value("fileName", "");
    m_tileData[id].slopeTiles.count = tileDataJSON[idx]["slopeTiles"].value("count", 1);
    m_tileData[id].slopeTiles.clippingWidth = tileDataJSON[idx]["slopeTiles"].value("clip_width", 0);
    m_tileData[id].slopeTiles.clippingHeight = tileDataJSON[idx]["slopeTiles"].value("clip_height", 0);

    if (!m_tileData[id].slopeTiles.fileName.empty())
    {
      ResourcesManager::instance().loadTexture(id, m_tileData[id].slopeTiles.fileName, TileMap::SLOPES);
    }
  }
}