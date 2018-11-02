#include "cell.hxx"

#include "basics/resources.hxx"
#include "tile.hxx"

#include "basics/log.hxx"

Cell::Cell(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates)), _tileID(14)
{
  _sprite = std::make_unique<Sprite>(_tileID, _isoCoordinates);
}

void Cell::increaseHeightOfCell()
{
  int height = _isoCoordinates.height;

  if (height < _maxCellHeight)
  {
    _isoCoordinates.height++;
    _sprite->setTileIsoCoordinates(_isoCoordinates);
  }
}

void Cell::decreaseHeightOfCell()
{
  int height = _isoCoordinates.height;

  if (height > 0)
  {
    _isoCoordinates.height--;
    _sprite->setTileIsoCoordinates(_isoCoordinates);
  }
}

void Cell::render() { _sprite->render(); }

void Cell::setElevationBitmask(unsigned int bitmask)
{
  Tile tile;
  //tile.test(bitmask);

  _elevationBitmask = bitmask;
  _sprite->setTexture(tile.getTexture(bitmask));
}
