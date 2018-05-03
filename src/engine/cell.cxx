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

  _tileID = -1;
  _position = 0;

  // if it's a floor cell, draw sprite based on neighbor tile height





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

  // Check for surrounding tiles everytime we render
  Point coords;
  Sprite* tile;

  int numElevatedNeighbors = 0;

  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  printf("Count neighbors: %d", _neighbors.size());


  for (int i = 0; i < _neighbors.size(); i++)
  {
    if (_neighbors[i] != nullptr)
    {
      coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();

        _neighbors[i]->determineTile();
  
      if (coords.getHeight() == tileHeight )
      {
        numElevatedNeighbors++;
        tile = new Sprite("resources/images/floor/floor.png", coords, _renderer, _window);
        printf("Found elevated tile at %d,%d\n", coords.getX(), coords.getY());
        _neighbors[i]->setSprite(tile);
      }
      else
      {
        //tile = new Sprite("resources/images/floor/floor_" + std::to_string(i) + ".png", coords, _renderer, _window);

      }
      //printf("Setting tile %d to coord %d,%d\n", i, coords.getX(), coords.getY());
        

    }
  }
  if (numElevatedNeighbors >= 1)
  {
    // TODO: Check if this should be handled here or below.
    _sprite = new Sprite("resources/images/floor/floor.png", _sprite->getTileIsoCoordinates(), _renderer, _window);
  }




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

  int numberOfNeighbors = 1;
  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  for (int i = 0; i < _neighbors.size(); i++)
  {
    if (_neighbors[i] != nullptr)
    {
      Point coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if (coords.getHeight() > tileHeight)
      {

        if (i == 0)
          _position |= BOTTOM_LEFT;
        if (i == 1)
          _position |= LEFT;
        if (i == 2)
          _position |= TOP_LEFT;
        if (i == 3)
          _position |= BOTTOM;
        //if (i == 4)
          // TODO: Center Tile. When the cell array is implemented and it's at the map boundaries, a solid rock tile should be drawn here.
        if (i == 5)
          _position |= TOP;
        if (i == 6)
          _position |= BOTTOM_RIGHT;
        if (i == 7)
          _position |= RIGHT;
        if (i == 8)
          _position |= TOP_RIGHT;

        numberOfNeighbors++;
      }
    }
  }


  // elevation !!!!!!!!!!!

  auto it = tileIdToPosition.find(_position);

  if (it != tileIdToPosition.end())
  {
    _tileID = it->second;
  }
  else
  {
    // else elevate!
  }

  if (_tileID != -1)
  {
    _sprite = new Sprite("resources/images/floor/floor_" + std::to_string(_tileID) + ".png", getSprite()->getTileIsoCoordinates(), _renderer, _window);
  }
  else // no neighbors found
  {
    _sprite = new Sprite("resources/images/floor/floor.png", getSprite()->getTileIsoCoordinates(), _renderer, _window);
  }

  // elevation !!!!!!!!!!!



}