#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix(int columns, int rows)
  : _cellMatrix((columns+1) * (rows+1)), _columns(columns+1), _rows(rows+1)
{
  _cellMatrix.reserve(_rows * _columns);
  initCells();
} 

std::vector<std::shared_ptr<Cell>> vectorMatrix::getCellNeighbors(int x, int y)
{
  std::vector<std::shared_ptr<Cell>> neighbors;
  neighbors.reserve(9);

  for (int rowIterator = -1; rowIterator <= 1; rowIterator++)
  {
    int currentRow = x + rowIterator;

    for (int columnIterator = -1; columnIterator <= 1; columnIterator++)
    {
      int currentColumn = y + columnIterator;

      // check if the neighbor is within bounds of the tilemap
      if ( currentRow >= 0 && currentRow < _rows && currentColumn >= 0 && currentColumn < _columns && !(currentRow == x && currentColumn == y) )
      {
        if ( _cellMatrix[currentRow * _columns + currentColumn] )
          neighbors.push_back(_cellMatrix[currentRow * _columns + currentColumn]);
      }
      else
      {
        neighbors.push_back(nullptr);
      }
    }
  }
  return neighbors;
}

void vectorMatrix::initCells()
{

  // initialize cell Matrix
  int z = 0;

  for (int x = 0; x <= Resources::settings.mapSize; x++)
  {
    for (int y = Resources::settings.mapSize; y >= 0; y--)
    {
      // cells need to be created at the correct vector "coordinates", or else the Z-Order will be broken
      z++;
      _cellMatrix[x * _columns + y] = std::shared_ptr<Cell>(new Cell(Point(x, y, z)));
    }
  }

  // neighbors must be set after all the cells are there
  for (auto &it : _cellMatrix)
  {
    it->setNeighbors(getCellNeighbors(it->getCoordinates().getX(), it->getCoordinates().getY()));
  }
}

void vectorMatrix::increaseHeight(Point isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();

  if ( height < Resources::settings.maxElevationHeight )
  {
    height += 1;
    isoCoordinates.setHeight(height);
    _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->increaseHeight();
    drawSurroundingTiles(_cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates());
  }
}

void vectorMatrix::decreaseHeight(Point isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();

  if ( height > 0 )
  {
    height -= 1;
    isoCoordinates.setHeight(height);
    _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->decreaseHeight();
    drawSurroundingTiles(_cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates());
  }
}



void vectorMatrix::drawSurroundingTiles(Point isoCoordinates)
{
  int tileHeight = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->getCoordinates().getHeight();
  //int tileHeight = _isoCoordinates.getHeight();
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
    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {

      if (_cellMatrix[it.first * _columns + it.second])
      {
        Point currentCoords = _cellMatrix[it.first * _columns + it.second]->getCoordinates();

        // there can't be a height difference greater then 1 between two map cells.
        if (tileHeight - _cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() > 1
          && Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE
          &&   i % 2)
        {
          increaseHeight(currentCoords);
          //_cellMatrix[it.first * _columns + it.second]->increaseHeight();
        }
        else if (tileHeight - _cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() < -1
          && Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER
          &&   i % 2)
        {
          decreaseHeight(currentCoords);
          //_cellMatrix[it.first * _columns + it.second]->decreaseHeight();
        }
        determineTile(_cellMatrix[it.first * _columns + it.second]->getCoordinates());
        //_cellMatrix[it.first * _columns + it.second]->determineTile();
      }
    }
    i++;
  }
  // call for this tile too. 
  //determineTile(isoCoordinates);
  //_cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()]->determineTile();
}


// WIP must be ported to vectorMatrix class too.
void vectorMatrix::determineTile(Point isoCoordinates)
{
  unsigned int _elevatedTilePosition = 0;
  std::shared_ptr<Cell> currentCell = _cellMatrix[isoCoordinates.getX() * _columns + isoCoordinates.getY()];
  int tileHeight = currentCell->getCoordinates().getHeight();
  //int tileHeight = isoCoordinates.getHeight();
  int newTileID = -2; // set to -2 to determine if it's necessary to set a new tile ID

  _elevatedTilePosition = getElevatedNeighborBitmask(isoCoordinates.getX(), isoCoordinates.getY());
  auto keyTileID = Resources::keyTileMap.find(_elevatedTilePosition);


  // ** create function for returning neighbor coordinates..
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

  // ***********++

  if ( keyTileID != Resources::keyTileMap.end() )
  {
    newTileID = keyTileID->second;
  }
    // special case: if both opposite neighbors are elevated, the center tile also gets elevated
    constexpr auto LEFT_and_RIGHT = ELEVATED_LEFT | ELEVATED_RIGHT;
    constexpr auto TOP_and_BOTTOM = ELEVATED_TOP | ELEVATED_BOTTOM;

    // LEFT-RIGHT / TOP-BOTTOM combinations are handled here because there are too many possible combinations
    // check if it could help to also handle the other diagobal combinations here too.
    if (((_elevatedTilePosition & LEFT_and_RIGHT) == LEFT_and_RIGHT)
      || ((_elevatedTilePosition & TOP_and_BOTTOM) == TOP_and_BOTTOM)
      || newTileID == -1)
    {
      if (Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE)
      {
        increaseHeight(currentCell->getCoordinates());
      }

      else if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
      {
        for (auto it : adjs)
        {
          if (_cellMatrix[it.first * _columns + it.second])
          {
            if (_cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() > tileHeight)
            {
              decreaseHeight(_cellMatrix[it.first * _columns + it.second]->getCoordinates());
            }
          }
        }
      }
      // Tile gets elevated, so tile id must be 14
      newTileID = 14;
    }

    if (newTileID != -2)
      currentCell->setTileID(newTileID);
    else
      LOG(LOG_ERROR) << "It seems there is no combination for bitmask: " << _elevatedTilePosition;
}

unsigned int vectorMatrix::getElevatedNeighborBitmask(int x, int y)
{
  unsigned int bitmask = 0;

  std::pair<int, int> adjs[8] = { 
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
  for (auto it : adjs)
  {
    if (_cellMatrix[it.first * _columns + it.second]->getCoordinates().getHeight() > _cellMatrix[x *_columns + y]->getCoordinates().getHeight()
      && _cellMatrix[it.first * _columns + it.second])
    {
      // for each found tile add 2 ^ i to the bitmask
      bitmask |= static_cast<unsigned int>(std::pow(2, i));
    }
    i++;
  }
  //LOG() << "returning: " << bitmask;
  return bitmask;
}