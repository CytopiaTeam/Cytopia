#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix(int columns, int rows)
  : _cellMatrix((columns+1) * (rows+1)), _columns(columns+1), _rows(rows+1)
{
  _cellMatrix.reserve(_rows * _columns);
  initCells();
} 

void vectorMatrix::initCells()
{
  int z = 0;

  // cells need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x <= Resources::settings.mapSize; x++)
  {
    for (int y = Resources::settings.mapSize; y >= 0; y--)
    {
      z++;
      //_cellMatrix.emplace(_cellMatrix.begin() + x * _columns + y,std::make_unique<Cell>(Point(x, y, z)));
      _cellMatrix[x * _columns + y] = std::make_shared<Cell>(Point(x, y, z));
    }
  }
}

void vectorMatrix::increaseHeightOfCell (const Point& isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();

  if ( height < Resources::settings.maxElevationHeight )
  {
    _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->increaseHeightOfCell();
    drawSurroundingTiles(_cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates());
  }
}

void vectorMatrix::decreaseHeightOfCell(const Point& isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();

  if ( height > 0 )
  {
    _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->decreaseHeightOfCell();
    drawSurroundingTiles(_cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates());
  }
}

void vectorMatrix::drawSurroundingTiles(const Point& isoCoordinates)
{
  int tileHeight = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();
  int i = 0;
  int x = isoCoordinates.getX();
  int y = isoCoordinates.getY();

  std::pair<int, int> adjs[9] = {
    std::make_pair(x - 1, y - 1),    // 6 = 2^6 = 64  = BOTTOM LEFT
    std::make_pair(x - 1, y),        // 2 = 2^2 = 4   = LEFT
    std::make_pair(x - 1, y + 1),    // 4 = 2^4 = 16  = TOP LEFT
    std::make_pair(x, y - 1),        // 1 = 2^1 = 2   = BOTTOM
    std::make_pair(x, y),            // center
    std::make_pair(x, y + 1),        // 0 = 2^0 = 1   = TOP 
    std::make_pair(x + 1, y - 1),     // 7 = 2^7 = 128 = BOTTOM RIGHT
    std::make_pair(x + 1, y),        // 3 = 2^3 = 8   = RIGHT
    std::make_pair(x + 1, y + 1)    // 5 = 2^5 = 32  = TOP RIGHT
  };

  for (auto it : adjs)
  {
    if ( it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns )
    {
      if ( _cellMatrix[it.first * _columns + it.second] )
      {
        Point currentCoords = _cellMatrix[it.first * _columns + it.second]->getCoordinates();

        // there can't be a height difference greater then 1 between two map cells.
        if ( tileHeight - _cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() > 1
        &&   Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE
        &&   i % 2 )
        {
          increaseHeightOfCell(currentCoords);
        }
        else if ( tileHeight - _cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() < -1
        &&        Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER
        &&        i % 2 )
        {
          decreaseHeightOfCell(currentCoords);
        }
        determineTileIDOfCell(_cellMatrix[it.first * _columns + it.second]->getCoordinates());
      }
    }
    i++;
  }
}

void vectorMatrix::determineTileIDOfCell(const Point& isoCoordinates)
{
  unsigned int _elevatedTilePosition = 0;
  int tileHeight = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();
  int newTileID = -2; // set to -2 to determine if it's necessary to set a new tile ID


  int x = isoCoordinates.getX();
  int y = isoCoordinates.getY();

  std::pair<int, int> adjecantCellCoordinates[9] = {
    std::make_pair(x - 1, y - 1),    // 6 = 2^6 = 64  = BOTTOM LEFT
    std::make_pair(x - 1, y),        // 2 = 2^2 = 4   = LEFT
    std::make_pair(x - 1, y + 1),    // 4 = 2^4 = 16  = TOP LEFT
    std::make_pair(x, y - 1),        // 1 = 2^1 = 2   = BOTTOM
    std::make_pair(x, y),            // center
    std::make_pair(x, y + 1),        // 0 = 2^0 = 1   = TOP 
    std::make_pair(x + 1, y - 1),     // 7 = 2^7 = 128 = BOTTOM RIGHT
    std::make_pair(x + 1, y),        // 3 = 2^3 = 8   = RIGHT
    std::make_pair(x + 1, y + 1)    // 5 = 2^5 = 32  = TOP RIGHT
  };


  _elevatedTilePosition = getElevatedNeighborBitmask(isoCoordinates);
  auto keyTileID = Resources::slopeTileIDMap.find(_elevatedTilePosition);

  if ( keyTileID != Resources::slopeTileIDMap.end() )
  {
    newTileID = keyTileID->second;
  }
  // special case: if both opposite neighbors are elevated, the center tile also gets elevated
  constexpr auto LEFT_and_RIGHT = ELEVATED_LEFT | ELEVATED_RIGHT;
  constexpr auto TOP_and_BOTTOM = ELEVATED_TOP | ELEVATED_BOTTOM;

  // LEFT-RIGHT / TOP-BOTTOM combinations are handled here because there are too many possible combinations
  // check if it could help to also handle the other diagobal combinations here too.
  if (( _elevatedTilePosition & LEFT_and_RIGHT) == LEFT_and_RIGHT
  || (  _elevatedTilePosition & TOP_and_BOTTOM) == TOP_and_BOTTOM
  ||    newTileID == -1 )
  {
    if ( Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE )
    {
      increaseHeightOfCell(_cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates());
    }
    else if ( Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER )
    {
      for (auto it : adjecantCellCoordinates)
      {
        if ( _cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() > tileHeight
        &&   _cellMatrix[it.first * _columns + it.second] )
        {
          decreaseHeightOfCell(_cellMatrix[it.first * _columns + it.second]->getCoordinates());
        }
      }
    }
    // Tile gets elevated, so tile id must be 14
    newTileID = 14;
  }

  if ( newTileID != -2 )
  {
    _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->setTileID(newTileID);
  }
  else
  {
    LOG(LOG_ERROR) << "It seems there is no combination for bitmask: " << _elevatedTilePosition;
  }
}

unsigned int vectorMatrix::getElevatedNeighborBitmask(const Point& isoCoordinates)
{
  unsigned int bitmask = 0;
  int x = isoCoordinates.getX();
  int y = isoCoordinates.getY();


  std::pair<int, int> adjecantCellCoordinates[8] = { 
    std::make_pair(x, y + 1),        // 0 = 2^0 = 1   = TOP 
    std::make_pair(x, y - 1),        // 1 = 2^1 = 2   = BOTTOM
    std::make_pair(x - 1, y),        // 2 = 2^2 = 4   = LEFT
    std::make_pair(x + 1, y),        // 3 = 2^3 = 8   = RIGHT
    std::make_pair(x - 1, y + 1),    // 4 = 2^4 = 16  = TOP LEFT
    std::make_pair(x + 1, y + 1),    // 5 = 2^5 = 32  = TOP RIGHT
    std::make_pair(x - 1, y - 1),    // 6 = 2^6 = 64  = BOTTOM LEFT
    std::make_pair(x + 1, y - 1)     // 7 = 2^7 = 128 = BOTTOM RIGHT
  };

  int i = 0;
  for (auto it : adjecantCellCoordinates)
  {
    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {
      if ( _cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() > _cellMatrix[x *_columns + y]->getCoordinates().getHeight()
      &&   _cellMatrix[it.first * _columns + it.second] )
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(std::pow(2, i));
      }
    }
    i++;
  }
  return bitmask;
}

std::vector<std::shared_ptr<Cell> > vectorMatrix::getNeighbors(const Point &isoCoordinates)
{
  int x = isoCoordinates.getX();
  int y = isoCoordinates.getY();

  std::vector<std::shared_ptr<Cell> > neighborCells;
  neighborCells.reserve(9);

  std::pair<int, int> adjecantCellCoordinates[9] = {
    std::make_pair(x - 1, y - 1),    // 6 = 2^6 = 64  = BOTTOM LEFT
    std::make_pair(x - 1, y),        // 2 = 2^2 = 4   = LEFT
    std::make_pair(x - 1, y + 1),    // 4 = 2^4 = 16  = TOP LEFT
    std::make_pair(x, y - 1),        // 1 = 2^1 = 2   = BOTTOM
    std::make_pair(x, y),            // center
    std::make_pair(x, y + 1),        // 0 = 2^0 = 1   = TOP 
    std::make_pair(x + 1, y - 1),     // 7 = 2^7 = 128 = BOTTOM RIGHT
    std::make_pair(x + 1, y),        // 3 = 2^3 = 8   = RIGHT
    std::make_pair(x + 1, y + 1)    // 5 = 2^5 = 32  = TOP RIGHT
  };

  for (auto it : adjecantCellCoordinates)
  {

    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {
      neighborCells.push_back(_cellMatrix[it.first * _columns + it.second]);
    }
    else
    {
      neighborCells.push_back(nullptr);
    }
  }
  return neighborCells;
}