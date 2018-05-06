#include "cell.hxx"

Cell::Cell()
{

}

Cell::Cell(Point isoCoordinates, Sprite* sprite, SDL_Renderer* renderer, SDL_Window* window)
{
  _sprite = sprite;
  _coordinates = isoCoordinates;

  _renderer = renderer;
  _window = window;

  _tileID = 14;
  _elevatedTilePosition = 0;
}

Cell::~Cell()
{

}

void Cell::setSprite(Sprite* sprite)
{
  _sprite = sprite;
}

Sprite* Cell::getSprite()
{
  return _sprite;
}

void Cell::setNeighbors(std::vector<Cell*> neighbors)
{
  _neighbors = neighbors;
}

void Cell::renderCell(Point cameraOffset, float zoom)
{

  _sprite->render(cameraOffset, zoom);
}


void Cell::drawSurroundingTiles(Point isoCoordinates)
{
  Sprite* tile;

  int numElevatedNeighbors = 0;
  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if ( _neighbors[i] != nullptr )
    {
      _neighbors[i]->determineTile();
    }
  }
  // call for this tile too. 
  determineTile();
}

void Cell::increaseHeight(int height)
{
  int tileHeight = _sprite->getTileIsoCoordinates().getHeight();
  _sprite->setHeight(1); // TODO: just one level is working now
  //_sprite->setHeight(tileHeight + 1);
  drawSurroundingTiles(_sprite->getTileIsoCoordinates());
}


bool Cell::hasElevatedNeighbors()
{
  bool elevatedNeighbors = false;

  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if ( _neighbors[i] != nullptr )
    {
      Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if ( coords.getHeight() == tileHeight )
      {
        elevatedNeighbors = true;
      }
    }
  }
  return elevatedNeighbors;
}

void Cell::determineTile()
{
  _elevatedTilePosition = 0;
  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  for (int i = 0; i < _neighbors.size(); i++) //determine TileID
  {
    if ( _neighbors[i] != nullptr )
    {
      Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if ( coords.getHeight() > tileHeight )
      {
        if ( i == 0 )
          _elevatedTilePosition |= ELEVATED_BOTTOM_LEFT;
        if ( i == 1 )
          _elevatedTilePosition |= ELEVATED_LEFT;
        if ( i == 2 )
          _elevatedTilePosition |= ELEVATED_TOP_LEFT;
        if ( i == 3 )
          _elevatedTilePosition |= ELEVATED_BOTTOM;
        if ( i == 5 )
          _elevatedTilePosition |= ELEVATED_TOP;
        if ( i == 6 )
          _elevatedTilePosition |= ELEVATED_BOTTOM_RIGHT;
        if ( i == 7 )
          _elevatedTilePosition |= ELEVATED_RIGHT;
        if ( i == 8 )
          _elevatedTilePosition |= ELEVATED_TOP_RIGHT;
      }
    }
  }


  auto keyTileID = keyTileMap.find(_elevatedTilePosition);

  if ( keyTileID != keyTileMap.end() )
  {
    _tileID = keyTileID->second;
  }

  // special case: if both opposite neighbors are elevated, the center tile also gets elevated
  if ( ((_elevatedTilePosition & ELEVATED_LEFT) && (_elevatedTilePosition & ELEVATED_RIGHT))  
    || ((_elevatedTilePosition & ELEVATED_TOP) && (_elevatedTilePosition & ELEVATED_BOTTOM)) 
    || _tileID == -1)
  {
    increaseHeight(1);
    _tileID = 14;
  }

  _sprite = new Sprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png", getSprite()->getTileIsoCoordinates(), _renderer, _window);
}