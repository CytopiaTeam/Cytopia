#include "cell.hxx"

Cell::Cell()
{

}

Cell::Cell(Point isoCoordinates)
{
  _isoCoordinates = isoCoordinates;

  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();

  // Default Floor sprite has tileID 14
  _tileID = 14;

  _elevatedTilePosition = 0;
  _sprite = new Sprite(_tileID, _isoCoordinates);
}


Cell::~Cell()
{

}

void Cell::setSprite(Sprite* sprite)
{
  _sprite = sprite;
}

Sprite* Cell::getSprite()
{
  return _sprite;
}

void Cell::setNeighbors(std::vector<Cell*> neighbors)
{
  _neighbors = neighbors;
}

void Cell::renderCell()
{
  if (_sprite != nullptr)
  {
    _sprite->setTileIsoCoordinates(_isoCoordinates);
    _sprite->render();
  }
}

Point Cell::getCoordinates()
{
  return _isoCoordinates;
}

void Cell::drawSurroundingTiles(Point isoCoordinates)
{
  int numElevatedNeighbors = 0;
  int tileHeight = _isoCoordinates.getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if ( _neighbors[i] != nullptr )
    {
      _neighbors[i]->determineTile();

      // there can't be a height difference greater then 1 between two map cells.
      if ( tileHeight - _neighbors[i]->getCoordinates().getHeight() > 1 
      &&   Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE
      &&   i % 2 )
      {
        _neighbors[i]->increaseHeight();
      }
      if (  tileHeight - _neighbors[i]->getCoordinates().getHeight() < -1 
      &&    Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER
      &&    i % 2) 
      {
        _neighbors[i]->decreaseHeight();
      }
    }
  }
  // call for this tile too. 
  determineTile();
}

void Cell::increaseHeight()
{
  int height = _isoCoordinates.getHeight();

  if ( height < _maxCellHeight )
  {
    _isoCoordinates.setHeight(_isoCoordinates.getHeight() + 1);
    drawSurroundingTiles(_isoCoordinates);
  }
}

void Cell::decreaseHeight()
{
  int height = _isoCoordinates.getHeight();
  
  if ( height > 0 )
  {
    _isoCoordinates.setHeight(height - 1);
    drawSurroundingTiles(_isoCoordinates);
  }
}

bool Cell::hasElevatedNeighbors()
{
  bool elevatedNeighbors = false;

  int tileHeight = _isoCoordinates.getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if ( _neighbors[i] != nullptr )
    {
      if ( _isoCoordinates.getHeight() == tileHeight )
      {
        elevatedNeighbors = true;
      }
    }
  }
  return elevatedNeighbors;
}

void Cell::determineTile()
{
  _elevatedTilePosition = 0;
  int tileHeight = _isoCoordinates.getHeight();

  for (int i = 0; i < _neighbors.size(); i++) //determine TileID
  {
    if ( _neighbors[i] != nullptr )
    {
      if ( _neighbors[i]->getCoordinates().getHeight() > tileHeight )
      {
        switch (i)
        {
          case 0: _elevatedTilePosition |= ELEVATED_BOTTOM_LEFT; break;
          case 1: _elevatedTilePosition |= ELEVATED_LEFT; break;
          case 2: _elevatedTilePosition |= ELEVATED_TOP_LEFT; break;
          case 3: _elevatedTilePosition |= ELEVATED_BOTTOM; break;
          case 5: _elevatedTilePosition |= ELEVATED_TOP; break;
          case 6: _elevatedTilePosition |= ELEVATED_BOTTOM_RIGHT; break;
          case 7: _elevatedTilePosition |= ELEVATED_RIGHT; break;
          case 8: _elevatedTilePosition |= ELEVATED_TOP_RIGHT; break;
        }
      }
    }
  }

  auto keyTileID = keyTileMap.find(_elevatedTilePosition);

  if ( keyTileID != keyTileMap.end() )
  {
    _tileID = keyTileID->second;
  }

  // special case: if both opposite neighbors are elevated, the center tile also gets elevated
  if ((( (_elevatedTilePosition & ELEVATED_LEFT) && (_elevatedTilePosition & ELEVATED_RIGHT) )
  ||   ( (_elevatedTilePosition & ELEVATED_TOP) && (_elevatedTilePosition & ELEVATED_BOTTOM) )
  ||      _tileID == -1 )
  &&      Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE)
  {
    increaseHeight();
    _tileID = 14;
  }

  if ((( (_elevatedTilePosition & ELEVATED_LEFT) && (_elevatedTilePosition & ELEVATED_RIGHT) )
  || (   (_elevatedTilePosition & ELEVATED_TOP)  && (_elevatedTilePosition & ELEVATED_BOTTOM) )
  ||      _tileID == -1 )
  &&      Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
  {
    for (int i = 0; i < _neighbors.size(); i++) //determine TileID
    {
      if ( _neighbors[i] != nullptr )
      {
        if ( _neighbors[i]->getCoordinates().getHeight() > tileHeight )
        {
          _neighbors[i]->decreaseHeight();
        }
      }
    }
    _tileID = 14;
  }
  _sprite->changeTexture(_tileID);
}

void Cell::setTileID(int tileID)
{
  _sprite->changeTexture(_tileID);
  _tileID = tileID;
}


bool Cell::isInCell(Point clickedCoords)

{
  Point screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  bool isClicked = false;

  float _zoomLevel = Resources::getZoomLevel();
  float tilesize = 32 * _zoomLevel;

  // calculate the center of the cell
  int cellCenterX = screenCoordinates.getX() + (tilesize * 0.5);
  int cellCenterY = screenCoordinates.getY() + (tilesize * 0.25);
    
  int distanceX = abs((clickedCoords.getX()) - cellCenterX) ;
  int distanceY = abs((clickedCoords.getY()) - cellCenterY) ;

  // Bug: Overlapping tile either triggers both tiles or returns 0,0
  if ((distanceX / (tilesize * 0.5)) + (distanceY / (tilesize * 0.25)) <= 1.0)
  {
    std::cout << "I AM CLICKED !!!!!!!!!!!!!!!!!";
    std::cout << "My ISO Coordinates are: " << _isoCoordinates.getX() << " , " << _isoCoordinates.getY() << std::endl;

    return true;
  }
  return isClicked;
}

bool Cell::checkCell(Point clickedCoords)
{
  Point screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  float _zoomLevel = Resources::getZoomLevel();
  float tilesize = 32 * _zoomLevel;

  // is already centered??!! if not here are the calculatons
  int centerX = screenCoordinates.getX() + (tilesize * 0.5 * _zoomLevel);

  int centerY = screenCoordinates.getY() + (tilesize * 0.75  * _zoomLevel);



  int dx = abs((clickedCoords.getX()) - centerX);

  int dy = abs((clickedCoords.getY()) - centerY);



  float calc = (dx / (tilesize * 0.5 )) + (dy / (tilesize * 0.25 ));

  if (calc <= 1.0) {



    std::cout << "--------------------" << std::endl;

    std::cout << "Calculated : " << calc << " My zoomlevel = " << _zoomLevel << std::endl;

    std::cout << "dx, dy : " << dx << " , " << dy << std::endl;

    std::cout << "Calculated dx value: " << (dx / (tilesize)) << std::endl;

    std::cout << "Calculated dx value: " << (dy / (tilesize)) << std::endl;

    std::cout << "My ISO Coordinates are: " << _isoCoordinates.getX() << " , " << _isoCoordinates.getY() << std::endl;

    std::cout << "My SCREEN Coordinates are: " << screenCoordinates.getX() << " , " << screenCoordinates.getY() <<

      "\t and my Center = " << centerX << " , " << centerY << std::endl;

    std::cout << "My CLICKED Coordinates are: " << clickedCoords.getX() << " , " << clickedCoords.getY() << std::endl;



    return true;

  }
}