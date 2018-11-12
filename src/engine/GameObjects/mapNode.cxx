#include "mapNode.hxx"
#include "../basics/log.hxx"

MapNode::MapNode(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates)), _tileID(14)
{
  _tileData = Tile::Instance().getTileData(_tileType);
  SDL_Rect clipRect;
  clipRect.x = _tileData->tiles.clippingWidth * _tileData->tiles.count;
  _sprite = std::make_unique<Sprite>(Tile::Instance().getTextureNew(_tileType, _tileMap), _isoCoordinates);
  _sprite->setClipRect({clipRect.x, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
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

void MapNode::render() { _sprite->renderNew(); }

void MapNode::setElevationBitmask(unsigned char bitmask)
{
  _elevationBitmask = bitmask;
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
  _orientationNew = Tile::Instance().caluclateOrientationNew(_elevationBitmask);
  _sprite->setOrientation(_orientationNew);

  SDL_Rect clipRect;

  if (_orientationNew == TileSlopes::DEFAULT_ORIENTATION)
  {
    _tileMap = TileMap::DEFAULT;
  }
  else
  {
    _tileMap = TileMap::SLOPES;
  }
  switch (_tileMap)
  {
  case TileMap::DEFAULT:
    clipRect.x = _tileData->tiles.clippingWidth;
    _sprite->setClipRect({0, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->tiles.count);
    break;
  case TileMap::CORNERS:
    clipRect.x = _tileData->cornerTiles.clippingWidth * (int)_orientationNew;
    _sprite->setClipRect({clipRect.x, 0, _tileData->cornerTiles.clippingWidth, _tileData->cornerTiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->cornerTiles.count);
    break;
  case TileMap::SLOPES:
    clipRect.x = _tileData->slopeTiles.clippingWidth * (int)_orientationNew;
    _sprite->setClipRect({clipRect.x, 0, _tileData->slopeTiles.clippingWidth, _tileData->slopeTiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->slopeTiles.count);
    break;
  }
  _sprite->setTextureNew(Tile::Instance().getTextureNew(_tileType, _tileMap));
}