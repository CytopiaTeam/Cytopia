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

void MapNode::setBitmask(unsigned char elevationBitmask, unsigned char tileTypeBitmask)
{
  _elevationBitmask = elevationBitmask;
  _tileTypeBitmask = tileTypeBitmask;
  updateTexture();
}

void MapNode::setTileType(const std::string &tileType)
{
  _tileType = tileType;
  updateTexture();
};

void MapNode::updateTexture()
{
  _tileData = Tile::Instance().getTileData(_tileType);
  _orientation = Tile::Instance().caluclateSlopeOrientation(_elevationBitmask);
  {
    //_orientation = Tile::Instance().caluclateTileOrientation(_tileTypeBitmask);
  }
  _sprite->setOrientation(_orientation);

  SDL_Rect clipRect;

  if (_orientation == TileSlopes::DEFAULT_ORIENTATION)
  {
    _tileMap = TileMap::DEFAULT;
    if (_tileType != "terrain")
    {
      _orientation = Tile::Instance().caluclateTileOrientation(_tileTypeBitmask);
    }
  }
  else
  {
    _tileMap = TileMap::SLOPES;
  }
  switch (_tileMap)
  {
  case TileMap::DEFAULT:
    clipRect.x = _tileData->tiles.clippingWidth * (int)_orientation;
    if (_tileType == "terrain")
    {
      _sprite->setClipRect({0, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
    }
    else
    {
      _sprite->setClipRect({clipRect.x, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
    }
    _sprite->setSpriteCount(_tileData->tiles.count);
    break;
  case TileMap::CORNERS:
    clipRect.x = _tileData->cornerTiles.clippingWidth * (int)_orientation;
    _sprite->setClipRect({clipRect.x, 0, _tileData->cornerTiles.clippingWidth, _tileData->cornerTiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->cornerTiles.count);
    break;
  case TileMap::SLOPES:
    clipRect.x = _tileData->slopeTiles.clippingWidth * (int)_orientation;
    _sprite->setClipRect({clipRect.x, 0, _tileData->slopeTiles.clippingWidth, _tileData->slopeTiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->slopeTiles.count);
    break;
  }
  _sprite->setTexture(Tile::Instance().getTexture(_tileType, _tileMap));
}