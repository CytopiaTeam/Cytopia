#include "mapNode.hxx"
#include "../basics/log.hxx"

MapNode::MapNode(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates))
{
  _sprite = std::make_unique<Sprite>(_isoCoordinates);
  updateTexture();
}

void MapNode::increaseHeight()
{
  int height = _isoCoordinates.height;

  if (height < _maxHeight)
  {
    _isoCoordinates.height++;
    _sprite->setCoordinates(_isoCoordinates);
  }
}

void MapNode::decreaseHeight()
{
  int height = _isoCoordinates.height;

  if (height > 0)
  {
    _isoCoordinates.height--;
    _sprite->setCoordinates(_isoCoordinates);
  }
}

void MapNode::render() { _sprite->render(); }

void MapNode::setBitmask(unsigned char elevationBitmask, unsigned char tileIDBitmask)
{
  _elevationBitmask = elevationBitmask;
  _tileIDBitmask = tileIDBitmask;
  updateTexture();
}

void MapNode::setTileID(const std::string &tileID)
{
  _previousTileID = _tileID;
  _tileID = tileID;
  updateTexture();
};

void MapNode::updateTexture()
{
  _tileData = Tile::Instance().getTileData(_tileID);
  _orientation = Tile::Instance().caluclateSlopeOrientation(_elevationBitmask);

  SDL_Rect clipRect;

  if (_orientation == TileSlopes::DEFAULT_ORIENTATION)
  {
    _tileMap = TileMap::DEFAULT;
    if (_tileData->type != "terrain" || _tileData->type != "road" || _tileData->type != "water")
    {
      _orientation = Tile::Instance().caluclateTileOrientation(_tileIDBitmask);
    }
  }
  else
  {
    _tileMap = TileMap::SLOPES;
  }

  _sprite->setOrientation(_orientation);

  switch (_tileMap)
  {
  case TileMap::DEFAULT:
    clipRect.x = _tileData->tiles.clippingWidth * (int)_orientation;
    _clippingWidth = _tileData->tiles.clippingWidth;
    if (_tileID == "terrain")
    {
      clipRect.x = 0;
      _sprite->setClipRect({0, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
    }
    else
    {
      _sprite->setClipRect({clipRect.x, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
    }
    _spriteCount = _tileData->tiles.count;
    break;
  case TileMap::CORNERS:
    _clippingWidth = _tileData->cornerTiles.clippingWidth;
    clipRect.x = _tileData->cornerTiles.clippingWidth * (int)_orientation;
    _sprite->setClipRect({clipRect.x, 0, _tileData->cornerTiles.clippingWidth, _tileData->cornerTiles.clippingHeight});
    _spriteCount = _tileData->cornerTiles.count;
    break;
  case TileMap::SLOPES:
    _clippingWidth = _tileData->slopeTiles.clippingWidth;
    clipRect.x = _tileData->slopeTiles.clippingWidth * (int)_orientation;
    _sprite->setClipRect({clipRect.x, 0, _tileData->slopeTiles.clippingWidth, _tileData->slopeTiles.clippingHeight});
    _spriteCount = _tileData->slopeTiles.count;
    break;
  }
  if (clipRect.x >= _spriteCount * _clippingWidth)
  {
    _tileID = _previousTileID;
    updateTexture();
  }
  _sprite->setSpriteCount(_spriteCount);
  _sprite->setTexture(Tile::Instance().getTexture(_tileID, _tileMap));
}