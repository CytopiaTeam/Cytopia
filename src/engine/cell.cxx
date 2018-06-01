#include "cell.hxx"

Cell::Cell(Point isoCoordinates) : _isoCoordinates(isoCoordinates), _tileID(14), _elevatedTilePosition(0)
{
  _sprite = std::shared_ptr<Sprite>(new Sprite(_tileID, _isoCoordinates));
}

void Cell::setNeighbors(std::vector<std::shared_ptr<Cell>> neighbors)
{
  _neighbors = neighbors;
}

void Cell::drawSurroundingTiles(Point isoCoordinates)
{
  int tileHeight = _isoCoordinates.getHeight();
  int i = 0;

  for (auto it : _neighbors)
  {
    if ( it  )
    {
      it->determineTile();

      // there can't be a height difference greater then 1 between two map cells.
      if ( tileHeight - it->getCoordinates().getHeight() > 1 
      &&   Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE
      &&   i % 2 )
      {
        _neighbors[i]->increaseHeight();
      }
      if ( tileHeight - it->getCoordinates().getHeight() < -1 
      &&   Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER
      &&   i % 2 ) 
      {
        it->decreaseHeight();
      }
    }
    i++;
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
    //drawSurroundingTiles(_isoCoordinates);
    _sprite->setTileIsoCoordinates(_isoCoordinates);
  }
}

void Cell::decreaseHeight()
{
  int height = _isoCoordinates.getHeight();
  
  if ( height > 0 )
  {
    _isoCoordinates.setHeight(height - 1);
    //drawSurroundingTiles(_isoCoordinates);
    _sprite->setTileIsoCoordinates(_isoCoordinates);
  }
}

void Cell::determineTile()
{
  _elevatedTilePosition = 0;
  int tileHeight = _isoCoordinates.getHeight();



  for (int i = 0; i < _neighbors.size(); i++) //determine TileID
  {
    if ( _neighbors[i] )
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

  auto keyTileID = Resources::keyTileMap.find(_elevatedTilePosition);

  if ( keyTileID != Resources::keyTileMap.end() )
  {
    _tileID = keyTileID->second;
  }

  // special case: if both opposite neighbors are elevated, the center tile also gets elevated
  constexpr auto LEFT_and_RIGHT = ELEVATED_LEFT | ELEVATED_RIGHT;
  constexpr auto TOP_and_BOTTOM = ELEVATED_TOP | ELEVATED_BOTTOM;

  if (( (_elevatedTilePosition & LEFT_and_RIGHT) == LEFT_and_RIGHT )
  ||  ( (_elevatedTilePosition & TOP_and_BOTTOM) == TOP_and_BOTTOM )
  ||      _tileID == -1 )
  {
    if ( Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE )
    {
      increaseHeight();
     _tileID = 14;
    }

    else if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
    {
      for (int i = 0; i < _neighbors.size(); i++)
      {
        if (_neighbors[i])
        {
          if (_neighbors[i]->getCoordinates().getHeight() > tileHeight)
          {
            _neighbors[i]->decreaseHeight();
          }
        }
      }
      _tileID = 14;
    }
  }

  _sprite->changeTexture(_tileID);
}