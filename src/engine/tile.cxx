#include "tile.hxx"

#include <bitset>

#include "basics/log.hxx"
#include "textureManager.hxx"

#include "../ThirdParty/json.hxx"

using json = nlohmann::json;

Tile::Tile() { init(); }

SDL_Texture *Tile::getTexture(const std::string &id, size_t tileMapType)
{
  return TextureManager::Instance().getTileTexture(id, tileMapType);
}

TileInformation *Tile::getTileData(const std::string &id)
{
  if (tileData.count(id))
  {
    return &tileData[id];
  }
  else
  {
    LOG(LOG_ERROR) << "Non existing id is beeing accessed at Tile::getTileData(id) " << id;
  }

  return nullptr;
}

size_t Tile::caluclateOrientationNew(unsigned char bitMaskElevation)
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
    orientation = TileSlopes::DEFAULT_ORIENTATION;
  }
  // special cases
  else if (elevationMask.test(3) && elevationMask.test(6))
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

  else
  {
    LOG(LOG_ERROR) << "No Combination for bitmask " << elevationMask.to_string() << " found! This should not have happened";
  }
  return orientation;
}

std::string Tile::caluclateOrientation(unsigned char bitMaskElevation)
{
  std::string orientation;
  std::bitset<8> elevationMask(bitMaskElevation);

  // Bits (tile faces elevated directions):
  // 0 = 2^0 = 1   = N - TOP
  // 1 = 2^1 = 2   = S - BOTTOM
  // 2 = 2^2 = 4   = W - LEFT
  // 3 = 2^3 = 8   = E - RIGHT
  // 4 = 2^4 = 16  = TOP LEFT
  // 5 = 2^5 = 32  = TOP RIGHT
  // 6 = 2^6 = 64  = BOTTOM LEFT
  // 7 = 2^7 = 128 = BOTTOM RIGHT
  // check for all combinations
  if (elevationMask.none())
  { // NONE
    orientation = "default";
  }

  // special cases
  else if (elevationMask.test(3) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = "s_and_w";
  }
  else if (elevationMask.test(2) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = "n_and_e";
  }
  else if (elevationMask.test(3) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = "n_and_w";
  }
  else if (elevationMask.test(2) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = "s_and_e";
  }

  else if (elevationMask.test(0) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = "n_and_e";
  }
  else if (elevationMask.test(1) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = "s_and_w";
  }
  else if (elevationMask.test(0) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = "n_and_w";
  }
  else if (elevationMask.test(1) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = "s_and_e";
  }

  // diagonal combinations
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && RIGHT
    orientation = "n_and_w";
  }
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && LEFT
    orientation = "n_and_e";
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && RIGHT
    orientation = "s_and_w";
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && LEFT
    orientation = "s_and_e";
  }

  // default directions
  else if (elevationMask.test(0))
  { // TOP
    orientation = "n";
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    orientation = "s";
  }
  else if (elevationMask.test(2))
  { // LEFT
    orientation = "e";
  }
  else if (elevationMask.test(3))
  { // RIGHT
    orientation = "w";
  }
  else if ((elevationMask.test(4) && elevationMask.test(7)) || (elevationMask.test(5) && elevationMask.test(6)))
  { // BOTTOM_RIGHT
    orientation = "center";
  }
  else if (elevationMask.test(5))
  { // TOP_LEFT
    orientation = "nw";
  }
  else if (elevationMask.test(4))
  { // TOP_RIGHT
    orientation = "ne";
  }
  else if (elevationMask.test(7))
  { // BOTTOM_LEFT
    orientation = "sw";
  }
  else if (elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = "se";
  }

  else
  {
    LOG(LOG_ERROR) << "No Combination for bitmask " << elevationMask.to_string() << " found! This should not have happened";
  }
  return orientation;
}

void Tile::init()
{
  json tileDataJSON;

  // Read JSON File.
  std::ifstream i("resources/data/TileDataNew.json");
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File "
                   << "resources/data/TileData.json"
                   << " does not exist! Cannot load settings from INI File!";
    return;
  }

  // check if json file can be parsed
  tileDataJSON = json::parse(i, nullptr, false);
  if (tileDataJSON.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File "
                   << "resources/data/TileData.json";
  }
  i.close();

  std::string key;

  size_t idx = 0;

  while (!tileDataJSON[idx].is_null())
  {
    std::string id;
    id = tileDataJSON[idx].value("id", "");
    tileData[id].author = tileDataJSON[idx].value("author", "");
    tileData[id].title = tileDataJSON[idx].value("title", "");
    tileData[id].description = tileDataJSON[idx].value("description", "");
    tileData[id].type = tileDataJSON[idx].value("type", "");
    tileData[id].price = tileDataJSON[idx].value("price", 0);
    tileData[id].water = tileDataJSON[idx].value("water", 0);
    tileData[id].drawGround = tileDataJSON[idx].value("draw ground", false);

    tileData[id].tiles.fileName = tileDataJSON[idx]["tiles"].value("fileName", "");
    tileData[id].tiles.count = tileDataJSON[idx]["tiles"].value("count", 1);
    tileData[id].tiles.clippingWidth = tileDataJSON[idx]["tiles"].value("clip_width", 0);
    tileData[id].tiles.clippingHeight = tileDataJSON[idx]["tiles"].value("clip_height", 0);

    if (!tileData[id].tiles.fileName.empty())
    {
      TextureManager::Instance().loadTexture(id, tileData[id].tiles.fileName, TileMap::DEFAULT);
    }

    if (!tileDataJSON[idx]["cornerTiles"].is_null())
    {
      tileData[id].cornerTiles.fileName = tileDataJSON[idx]["cornerTiles"].value("fileName", "");
      tileData[id].cornerTiles.count = tileDataJSON[idx]["cornerTiles"].value("count", 1);
      tileData[id].cornerTiles.clippingWidth = tileDataJSON[idx]["cornerTiles"].value("clip_width", 0);
      tileData[id].cornerTiles.clippingHeight = tileDataJSON[idx]["cornerTiles"].value("clip_height", 0);

      if (!tileData[id].cornerTiles.fileName.empty())
      {
        TextureManager::Instance().loadTexture(id, tileData[id].cornerTiles.fileName, TileMap::CORNERS);
      }
    }

    if (!tileDataJSON[idx]["slopeTiles"].is_null())
    {

      tileData[id].slopeTiles.fileName = tileDataJSON[idx]["slopeTiles"].value("fileName", "");
      tileData[id].slopeTiles.count = tileDataJSON[idx]["slopeTiles"].value("count", 1);
      tileData[id].slopeTiles.clippingWidth = tileDataJSON[idx]["slopeTiles"].value("clip_width", 0);
      tileData[id].slopeTiles.clippingHeight = tileDataJSON[idx]["slopeTiles"].value("clip_height", 0);

      if (!tileData[id].slopeTiles.fileName.empty())
      {
        TextureManager::Instance().loadTexture(id, tileData[id].slopeTiles.fileName, TileMap::SLOPES);
      }
    }

    idx++;
  }
}
