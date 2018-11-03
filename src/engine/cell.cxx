#include "cell.hxx"

#include "basics/resources.hxx"

#include "basics/log.hxx"

Cell::Cell(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates)), _tileID(14)
{
  _sprite = std::make_unique<Sprite>(Tile::getTexture(_type, _orientation), _isoCoordinates);
}

void Cell::increaseHeightOfCell()
{
  int height = _isoCoordinates.height;

  if (height < _maxCellHeight)
  {
    _isoCoordinates.height++;
    _sprite->setCoordinates(_isoCoordinates);
  }
}

void Cell::decreaseHeightOfCell()
{
  int height = _isoCoordinates.height;

  if (height > 0)
  {
    _isoCoordinates.height--;
    _sprite->setCoordinates(_isoCoordinates);
  }
}

void Cell::render() { _sprite->render(); }

void Cell::setElevationBitmask(unsigned char bitmask)
{
  _elevationBitmask = bitmask;
  _orientation = Tile::caluclateOrientation(bitmask);
  _sprite->setTexture(Tile::getTexture(_type, _orientation));
}
