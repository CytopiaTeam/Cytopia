#include "cell.hxx"

Cell::Cell()
{

}

Cell::Cell(Point isoCoordinates) : _isoCoordinates(isoCoordinates)
{
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

Sprite* Cell::getSprite()
{
  return _sprite;
}

void Cell::setNeighbors(std::vector<Cell*> neighbors)
{
  _neighbors = std::move(neighbors);
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
    for (int i = 0; i < _neighbors.size(); i++)
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

int Cell::getTileID()
{
  return _tileID;
}