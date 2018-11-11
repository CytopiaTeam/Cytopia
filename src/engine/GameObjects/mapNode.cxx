#include "mapNode.hxx"

MapNode::MapNode(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates)), _tileID(14)
{
  _sprite = std::make_unique<Sprite>(Tile::Instance().getTextureNew(_tileType, _tileMap), _isoCoordinates);
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
  _tileData = Tile::Instance().getTileData(_tileType);
  _elevationBitmask = bitmask;
  _orientationNew = Tile::Instance().caluclateOrientationNew(bitmask);
  _sprite->setTexture(Tile::Instance().getTextureNew(_tileType, _tileMap));
  _sprite->setOrientation(_orientationNew);

  switch (_tileMap)
  {
  case TileMap::DEFAULT:
    _sprite->setClipRect({0, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->tiles.count);
    break;
  case TileMap::CORNERS:
    _sprite->setClipRect({0, 0, _tileData->cornerTiles.clippingWidth, _tileData->cornerTiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->cornerTiles.count);
    break;
  case TileMap::SLOPES:
    _sprite->setClipRect({0, 0, _tileData->slopeTiles.clippingWidth, _tileData->slopeTiles.clippingHeight});
    _sprite->setSpriteCount(_tileData->slopeTiles.count);
    break;
  }
}
