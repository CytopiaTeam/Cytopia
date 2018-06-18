#include "cell.hxx"

Cell::Cell(const Point &isoCoordinates) : _isoCoordinates(isoCoordinates), _tileID(14)
{
  _sprite = std::make_shared<Sprite>(_tileID, _isoCoordinates);
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
