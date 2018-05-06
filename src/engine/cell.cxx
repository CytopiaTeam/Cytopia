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
  _position = 0;
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
    if (_neighbors[i] != nullptr)
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
  _sprite->setHeight(tileHeight + 1);
  drawSurroundingTiles(_sprite->getTileIsoCoordinates());

}

bool Cell::hasElevatedNeighbors()
{
  bool elevatedNeighbors = false;

  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if (_neighbors[i] != nullptr)
    {
      Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if (coords.getHeight() == tileHeight)
      {
        elevatedNeighbors = true;
      }
    }
  }
  return elevatedNeighbors;
}

void Cell::determineTile()
{
  _position = 0;
  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  for (int i = 0; i < _neighbors.size(); i++) //determine TileID
  {
    if (_neighbors[i] != nullptr)
    {
      Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if ( coords.getHeight() > tileHeight )
      {
        if (i == 0)
          _position |= BOTTOM_LEFT;
        if (i == 1)
          _position |= LEFT;
        if (i == 2)
          _position |= TOP_LEFT;
        if (i == 3)
          _position |= BOTTOM;
        if (i == 5)
          _position |= TOP;
        if (i == 6)
          _position |= BOTTOM_RIGHT;
        if (i == 7)
          _position |= RIGHT;
        if (i == 8)
          _position |= TOP_RIGHT;
      }
      // Enable for Cell Debugging output
      //printf("I am %d, %d the neighbor of %d, %d and my position is %d\nMy Height is %d while my neighbors height is %d \n", getSprite()->getTileIsoCoordinates().getX(), getSprite()->getTileIsoCoordinates().getY(), _neighbors[i]->getSprite()->getTileIsoCoordinates().getX(), _neighbors[i]->getSprite()->getTileIsoCoordinates().getY(), _position, tileHeight, coords.getHeight());
    }
  }


  // elevation !!!!!!!!!!!

  auto keyTileID = keyTileMap.find(_position);

  if (keyTileID != keyTileMap.end())
  {
    _tileID = keyTileID->second;
  }
  else
  {
    // For debugging purposes
    printf("WARNING: Tile combination at %d, %d is not in the map!", getSprite()->getTileIsoCoordinates().getX(), getSprite()->getTileIsoCoordinates().getY());
  }

  // special case: if both opposite neighbors are elevated, the center tile also gets elevated
  if ( ((_position & LEFT) && (_position & RIGHT)) || ((_position & TOP) && (_position & BOTTOM)))
  {
    increaseHeight(1);
    _tileID = 14;
    printf("Position %d\n", _position);
  }

  _sprite = new Sprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png", getSprite()->getTileIsoCoordinates(), _renderer, _window);

  // elevation !!!!!!!!!!!
}