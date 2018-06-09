#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix(int columns, int rows)
  : _columns(columns+1), _rows(rows+1)
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
      _cellMatrix.push_back(std::make_shared<Cell>(Point(x, y, z)));
      //_cellMatrix[x * _columns + y] = std::make_shared<Cell>(Point(x, y, z));
    }
  }


  // Debug Matrix
      LOG() << "Matrix";
  for (int x = 0; x <= Resources::settings.mapSize; x++)
    for (int y = 0; y <= Resources::settings.mapSize; y++)
    {
        LOG() << _cellMatrix[x * _columns + y]->getCoordinates().getX() << ", " << _cellMatrix[x * _columns + y]->getCoordinates().getY();
    }
  LOG()<< "-----------------";
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
    if ( it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns )
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

    if ( it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns )
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



void vectorMatrix::rotateMatrixNew()
{

  std::vector<std::shared_ptr<Cell> > rotatedMatrix(_cellMatrix.size());



  // transpose
  for (int i = 0; i<_rows; i++)
    for (int j = i; j < _columns; j++)
    {
      //LOG() << "Transposing " << i << ", " << j << " to " << j << ", " << i;
      _cellMatrix[i * _columns + j] = _cellMatrix[j * _columns + i];
      //rotatedMatrix[i * _columns + j] = _cellMatrix[j * _columns + i];

    }
  //// reverse
  for (int i = 0; i< _rows; i++)
    for (int j = 0, k = _columns - 1; j<k; j++, k--)
      _cellMatrix[i * _columns + j] = _cellMatrix[k * _columns + i];

  //_cellMatrix.swap(rotatedMatrix);


}


void vectorMatrix::rotateMatrix()
{
  std::vector<std::shared_ptr<Cell> > rotatedMatrix(_cellMatrix.size());
  std::vector<std::shared_ptr<Cell> > rotatedMatrixCC(_cellMatrix.size());
  //for (int x = 0; x < _rows; x++)
  //{
  //  for (int y = 0; y < _columns; y++)
  //  {
  //    int angle = 90;
  //    int xRotation = (cos(angle) * x + (-sin(angle) * y));
  //    int yRotation = (sin(angle) * x + (cos(angle) * y));
  //    
  //    std::shared_ptr<Cell> cellPtr(_cellMatrix[xRotation, yRotation]);
  //    //std::shared_ptr<Cell> cellPtr (_cellMatrix[(_cellMatrix.size() - y - 1)* _columns + x]);
  //    //std::shared_ptr<Cell> cellPtr(_cellMatrix[1]);
  //    rotatedMatrix.push_back(cellPtr);
  //  }
  //}


  // Need to allocate an array to store the transform
  //std::vector<IVec> temp(p[0].size(), IVec(p.size()));

  int count = 0;        // count variable
  int count2 = 0;      // 2nd count variable

                       //p_myImage2[offset].pixel = p_myImage[width * (height - 1 - y) + x].pixel;

  for (int x = 0; x < _rows; x++)
  {
    //for (int y = _columns; y >= 0; y--)
    for (int y = 0; y < _columns; y++)
    {
      // correctly rotate all elements of the matrix, but not the coordinates of the cells itself!
      // but with the elements rotated, the draw order is broken...

      int rotY = _columns - 1 - y;
      int rotCounterclockX = _columns - 1 - x;

      //rotatedMatrix[x * _columns + y] = _cellMatrix[_rows * (_columns - 1 - y) + x];
      //rotatedMatrix[x * _columns + y] = _cellMatrix[rotY *_columns + x];
      //rotatedMatrixCC[x * _columns + y] = _cellMatrix[y *_columns + rotCounterclockX];

      //Point myP = rotatedMatrix[x * _columns + y]->getCoordinates();
      //Point myPCC = rotatedMatrixCC[y *_columns + rotCounterclockX]->getCoordinates();
      //LOG() << "rotate - setting old coords " << myP.getX() << ", " << myP.getY() << "to " << x << ", " << y;
      

      rotatedMatrix[x * _columns + y] = _cellMatrix[x + _columns * rotY];
      rotatedMatrixCC[x * _columns + y] = _cellMatrix[rotCounterclockX + _columns * y];
      rotatedMatrix[x * _columns + y]->setCoordinates(Point(x, rotY));
      rotatedMatrixCC[x * _columns + y]->setCoordinates(Point(rotCounterclockX, y));
      
      //rotatedMatrix[x * _columns + y]->setCoordinates(Point(x, y, myP.getZ(), myP.getHeight()));
     
      //rotatedMatrixCC[x * _columns + y]->setCoordinates(Point(x, y, myPCC.getZ(), myPCC.getHeight()));
      //rotatedMatrix[x * _columns + y]->setNeighbors(getCellNeighbors(rotY, x));
      //Point myP = _cellMatrix[x * _columns + y]->getCoordinates();
      //_cellMatrix[x * _columns + y]->setCoordinates(Point(x, rotY, myP.getZ(), myP.getHeight()));
      //// Clockwise rotation
      //int index3 = _columns - count -1 ;
      //int index4 = _rows - count - 1;
      ////temp[count2][temp[0].size() - count - 1] = p[count][count2];
      //int x = index4;
      //int y = index3;
      //rotatedMatrix[x * _columns + y] = _cellMatrix[count * _columns + count2];
      // Counter-clockwise rotation
      //temp[temp.size()-count2-1][count] = p[count][count2];
    }
  }

  _cellMatrix.swap(rotatedMatrix);

}