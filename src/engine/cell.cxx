#include "cell.hxx"

Cell::Cell()
{

}

Cell::Cell(Point isoCoordinates, Sprite* sprite)
{
  _sprite = sprite;
  _coordinates = isoCoordinates;

  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();

  // TODO: Sprites are partially handled via tileID. This should be read from a json file in the resources directory.  
  _tileID = 14; 
  _elevatedTilePosition = 0;
  setTileID(_tileID);
}

Cell::Cell(Point isoCoordinates, int tileID)
{
  _sprite = nullptr;
  _coordinates = isoCoordinates;

  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();

  // TODO: handle sprites in this class. initialize with tile id.  
  _tileID = 14;

  _elevatedTilePosition = 0;
  setTileID(_tileID);
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
    // TODO: Is this necessary?
    _sprite->setTileIsoCoordinates(_coordinates);
    _sprite->render();
  }
}

Point Cell::getCoordinates()
{
  return _coordinates;
}

void Cell::drawSurroundingTiles(Point isoCoordinates)
{
  Sprite* tile;

  int numElevatedNeighbors = 0;
  int tileHeight = _coordinates.getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if ( _neighbors[i] != nullptr )
    {
      _neighbors[i]->determineTile();

      // there can't be a height difference greater then 1 between two map cells.
      if ((tileHeight - _neighbors[i]->getCoordinates().getHeight()) > 1 && i % 2)
      {
        _neighbors[i]->increaseHeight(1);
      }
    }
  }
  // call for this tile too. 
  determineTile();
}

void Cell::increaseHeight(int height)
{
  _coordinates.setHeight(_coordinates.getHeight() + 1);
  drawSurroundingTiles(_coordinates);
}

bool Cell::hasElevatedNeighbors()
{
  bool elevatedNeighbors = false;

  int tileHeight = _coordinates.getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if ( _neighbors[i] != nullptr )
    {
      //Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if ( _coordinates.getHeight() == tileHeight )
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
  int tileHeight = _coordinates.getHeight();

  for (int i = 0; i < _neighbors.size(); i++) //determine TileID
  {
    if ( _neighbors[i] != nullptr )
    {
      //if (coords.getHeight() > tileHeight)
      //Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
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
  if ( ((_elevatedTilePosition & ELEVATED_LEFT) && (_elevatedTilePosition & ELEVATED_RIGHT)) ||
       ((_elevatedTilePosition & ELEVATED_TOP) && (_elevatedTilePosition & ELEVATED_BOTTOM)) ||
       _tileID == -1)
  {
    increaseHeight(1);
    _tileID = 14;
  }

  //_sprite->changeSprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png");
  _sprite = new Sprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png", _coordinates);
}

void Cell::setTileID(int tileID)
{
  _tileID = tileID;
  //_sprite->changeSprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png");
  _sprite = new Sprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png", _coordinates);
}