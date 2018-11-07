#include "mapNode.hxx"

#include "../tile.hxx"

MapNode::MapNode(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates)), _tileID(14)
{
  _sprite = std::make_unique<Sprite>(Tile::getTexture(_type, _orientation), _isoCoordinates);
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

void MapNode::setElevationBitmask(unsigned char bitmask)
{
  _elevationBitmask = bitmask;
  _orientation = Tile::caluclateOrientation(bitmask);
  _sprite->setTexture(Tile::getTexture(_type, _orientation));
}
